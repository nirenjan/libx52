/*
 * Method descriptors, server dispatch, client routing.
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <localipc/lipc.h>

#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct lipc_client_inflight {
    uint32_t tid;
    uint16_t request_id;
    lipc_client_completion_cb on_complete;
    void *complete_user;
    struct lipc_client_inflight *next;
} lipc_client_inflight;

struct lipc_client {
    pthread_mutex_t lock;
    pthread_mutex_t io_lock;
    pthread_rwlock_t routes_lock;
    uint32_t next_tid;
    uint32_t max_payload;
    lipc_frame_reader reader;
    uint8_t *rx_payload;
    lipc_client_route_table routes;
    lipc_client_reply_handler *reply_handlers;
    lipc_client_notify_handler *notify_handlers;
    void *route_user;
    lipc_client_inflight *inflight;
};

typedef struct lipc_server_client {
    int fd;
    lipc_frame_reader reader;
    uint8_t *rx_payload;
} lipc_server_client;

struct lipc_server {
    pthread_mutex_t lock;
    pthread_rwlock_t handlers_lock;
    int listen_fd;
    int wake_rd;
    int wake_wr;
    int stop_requested;
    uint32_t max_payload;
    lipc_server_handler *handlers;
    size_t nhandlers;
    void *handler_user;
    lipc_server_client *clients;
    size_t nclients;
    size_t clients_cap;
};

static void lipc_server_client_close(lipc_server_client *client)
{
    if (!client) {
        return;
    }
    if (client->fd >= 0) {
        close(client->fd);
        client->fd = -1;
    }
    free(client->rx_payload);
    client->rx_payload = NULL;
}

static void lipc_server_remove_client(lipc_server *server, size_t idx)
{
    if (!server || idx >= server->nclients) {
        return;
    }
    lipc_server_client_close(&server->clients[idx]);
    if (idx + 1 < server->nclients) {
        server->clients[idx] = server->clients[server->nclients - 1];
    }
    server->nclients--;
}

static int lipc_server_find_client_index(const lipc_server *server, int fd, size_t *out_idx)
{
    if (!server || !out_idx) {
        return 0;
    }
    for (size_t i = 0; i < server->nclients; i++) {
        if (server->clients[i].fd == fd) {
            *out_idx = i;
            return 1;
        }
    }
    return 0;
}

static lipc_status lipc_server_add_client(lipc_server *server, int fd)
{
    lipc_server_client client;
    lipc_server_client *new_clients;

    if (!server || fd < 0) {
        return LIPC_BAD_HEADER;
    }
    if (lipc_socket_set_nonblocking(fd, 1) != LIPC_OK) {
        return LIPC_IO_ERROR;
    }

    memset(&client, 0, sizeof client);
    client.fd = fd;
    lipc_frame_reader_init(&client.reader);
    if (server->max_payload > 0) {
        client.rx_payload = malloc(server->max_payload);
        if (!client.rx_payload) {
            return LIPC_INTERNAL_ERROR;
        }
    }

    if (server->nclients == server->clients_cap) {
        size_t new_cap = server->clients_cap == 0 ? 8 : server->clients_cap * 2;
        new_clients = realloc(server->clients, new_cap * sizeof *new_clients);
        if (!new_clients) {
            free(client.rx_payload);
            return LIPC_INTERNAL_ERROR;
        }
        server->clients = new_clients;
        server->clients_cap = new_cap;
    }

    server->clients[server->nclients++] = client;
    return LIPC_OK;
}

static void lipc_server_drain_wake_fd(int wake_fd)
{
    uint8_t buf[64];
    ssize_t n;

    do {
        n = read(wake_fd, buf, sizeof buf);
    } while (n > 0 || (n < 0 && errno == EINTR));
}

static lipc_status lipc_server_reply_error(int fd, const lipc_header *hdr, lipc_status st)
{
    lipc_server_reply_ctx reply;

    if (!hdr || hdr->tid == 0) {
        return st;
    }
    reply.fd = fd;
    reply.request_hdr = *hdr;
    return lipc_server_reply(&reply, (uint16_t)st, 0, 0, NULL, 0);
}

lipc_server *lipc_server_create(int listen_fd, uint32_t max_payload,
    const lipc_server_handler *handlers, size_t nhandlers, void *handler_user)
{
    lipc_server *server;
    int wake_pipe[2];

    if (listen_fd < 0) {
        return NULL;
    }

    server = calloc(1, sizeof *server);
    if (!server) {
        return NULL;
    }

    if (pthread_mutex_init(&server->lock, NULL) != 0) {
        free(server);
        return NULL;
    }
    if (pthread_rwlock_init(&server->handlers_lock, NULL) != 0) {
        pthread_mutex_destroy(&server->lock);
        free(server);
        return NULL;
    }

    if (pipe(wake_pipe) != 0) {
        pthread_rwlock_destroy(&server->handlers_lock);
        pthread_mutex_destroy(&server->lock);
        free(server);
        return NULL;
    }
    if (lipc_socket_set_nonblocking(wake_pipe[0], 1) != LIPC_OK
        || lipc_socket_set_nonblocking(wake_pipe[1], 1) != LIPC_OK) {
        close(wake_pipe[0]);
        close(wake_pipe[1]);
        pthread_rwlock_destroy(&server->handlers_lock);
        pthread_mutex_destroy(&server->lock);
        free(server);
        return NULL;
    }

    server->listen_fd = listen_fd;
    server->wake_rd = wake_pipe[0];
    server->wake_wr = wake_pipe[1];
    server->max_payload = max_payload ? max_payload : LIPC_MAX_PAYLOAD_DEFAULT;
    if (handlers && nhandlers > 0) {
        server->handlers = calloc(nhandlers, sizeof *server->handlers);
        if (!server->handlers) {
            close(server->wake_rd);
            close(server->wake_wr);
            pthread_rwlock_destroy(&server->handlers_lock);
            pthread_mutex_destroy(&server->lock);
            free(server);
            return NULL;
        }
        memcpy(server->handlers, handlers, nhandlers * sizeof *handlers);
        server->nhandlers = nhandlers;
    }
    server->handler_user = handler_user;
    return server;
}

void lipc_server_destroy(lipc_server *server)
{
    if (!server) {
        return;
    }

    pthread_mutex_lock(&server->lock);

    for (size_t i = 0; i < server->nclients; i++) {
        lipc_server_client_close(&server->clients[i]);
    }
    free(server->clients);
    server->clients = NULL;
    server->nclients = 0;
    server->clients_cap = 0;

    if (server->listen_fd >= 0) {
        close(server->listen_fd);
        server->listen_fd = -1;
    }
    if (server->wake_rd >= 0) {
        close(server->wake_rd);
        server->wake_rd = -1;
    }
    if (server->wake_wr >= 0) {
        close(server->wake_wr);
        server->wake_wr = -1;
    }
    pthread_mutex_unlock(&server->lock);

    pthread_rwlock_wrlock(&server->handlers_lock);
    free(server->handlers);
    server->handlers = NULL;
    server->nhandlers = 0;
    server->handler_user = NULL;
    pthread_rwlock_unlock(&server->handlers_lock);

    pthread_rwlock_destroy(&server->handlers_lock);
    pthread_mutex_destroy(&server->lock);
    free(server);
}

lipc_status lipc_server_wake(lipc_server *server)
{
    uint8_t sig = 1;
    int wake_wr = -1;

    if (!server) {
        return LIPC_BAD_HEADER;
    }
    pthread_mutex_lock(&server->lock);
    wake_wr = server->wake_wr;
    pthread_mutex_unlock(&server->lock);
    if (wake_wr < 0) {
        return LIPC_BAD_HEADER;
    }

    for (;;) {
        ssize_t n = write(wake_wr, &sig, sizeof sig);
        if (n == (ssize_t)sizeof sig) {
            return LIPC_OK;
        }
        if (n < 0 && errno == EINTR) {
            continue;
        }
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return LIPC_OK;
        }
        return LIPC_IO_ERROR;
    }
}

lipc_status lipc_server_stop(lipc_server *server)
{
    if (!server) {
        return LIPC_BAD_HEADER;
    }

    pthread_mutex_lock(&server->lock);
    server->stop_requested = 1;
    pthread_mutex_unlock(&server->lock);
    return lipc_server_wake(server);
}

static int lipc_server_is_stopped(lipc_server *server)
{
    int stopped;

    pthread_mutex_lock(&server->lock);
    stopped = server->stop_requested;
    pthread_mutex_unlock(&server->lock);
    return stopped;
}

static lipc_status lipc_server_accept_pending(lipc_server *server)
{
    for (;;) {
        int cfd = accept(server->listen_fd, NULL, NULL);
        if (cfd < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return LIPC_OK;
            }
            return LIPC_IO_ERROR;
        }
        lipc_status st = lipc_server_add_client(server, cfd);
        if (st != LIPC_OK) {
            close(cfd);
            return st;
        }
    }
}

static lipc_status lipc_server_drain_client_frames(lipc_server *server, size_t idx)
{
    uint8_t scratch[4096];
    lipc_header hdr;
    size_t payload_len = 0;

    if (!server || idx >= server->nclients) {
        return LIPC_BAD_HEADER;
    }

    for (;;) {
        lipc_server_client *client = &server->clients[idx];
        const lipc_server_handler *handlers = NULL;
        size_t nhandlers = 0;
        void *handler_user = NULL;
        lipc_status st = lipc_frame_reader_read(client->fd, &client->reader, server->max_payload,
            scratch, sizeof scratch, NULL, &hdr, client->rx_payload, server->max_payload,
            &payload_len);
        if (st == LIPC_WOULD_BLOCK) {
            return LIPC_OK;
        }
        if (st != LIPC_OK) {
            return st;
        }

        pthread_rwlock_rdlock(&server->handlers_lock);
        handlers = server->handlers;
        nhandlers = server->nhandlers;
        handler_user = server->handler_user;
        pthread_rwlock_unlock(&server->handlers_lock);

        st = lipc_server_dispatch(handlers, nhandlers, handler_user,
            client->fd, &hdr, client->rx_payload, payload_len);
        if (st != LIPC_OK) {
            lipc_status reply_st = lipc_server_reply_error(client->fd, &hdr, st);
            if (reply_st != LIPC_OK && reply_st != st) {
                return reply_st;
            }
        }
    }
}

enum { LIPC_BROADCAST_MAX_FDS = 256 };

lipc_status lipc_server_broadcast_notify(lipc_server *server, uint16_t request, uint16_t status,
    uint16_t index, uint64_t value, const void *payload, size_t payload_len)
{
    int fds[LIPC_BROADCAST_MAX_FDS];
    size_t nfds;
    size_t i;

    if (!server) {
        return LIPC_BAD_HEADER;
    }
    if (payload_len > UINT32_MAX) {
        return LIPC_BAD_LENGTH;
    }

    pthread_mutex_lock(&server->lock);
    nfds = server->nclients;
    if (nfds > sizeof fds / sizeof fds[0]) {
        pthread_mutex_unlock(&server->lock);
        return LIPC_BAD_LENGTH;
    }
    for (i = 0; i < nfds; i++) {
        fds[i] = server->clients[i].fd;
    }
    pthread_mutex_unlock(&server->lock);

    lipc_header h = {
        .request = request,
        .status = status,
        .index = index,
        .tid = 0,
        .length = (uint32_t)payload_len,
        .value = value,
    };

    for (i = 0; i < nfds; i++) {
        (void)lipc_frame_write(fds[i], &h, payload, payload_len);
    }
    return LIPC_OK;
}

lipc_status lipc_server_run(lipc_server *server)
{
    int listen_fd;
    int wake_rd;

    if (!server) {
        return LIPC_BAD_HEADER;
    }
    pthread_mutex_lock(&server->lock);
    listen_fd = server->listen_fd;
    wake_rd = server->wake_rd;
    server->stop_requested = 0;
    pthread_mutex_unlock(&server->lock);
    if (listen_fd < 0 || wake_rd < 0) {
        return LIPC_BAD_HEADER;
    }
    if (lipc_socket_set_nonblocking(listen_fd, 1) != LIPC_OK) {
        return LIPC_IO_ERROR;
    }

    for (;;) {
        struct pollfd *pfds = NULL;
        size_t nfds = 0;
        int prc;
        lipc_status run_st = LIPC_OK;

        pthread_mutex_lock(&server->lock);
        nfds = server->nclients + 2;
        pfds = calloc(nfds, sizeof *pfds);
        if (!pfds) {
            pthread_mutex_unlock(&server->lock);
            return LIPC_INTERNAL_ERROR;
        }

        pfds[0].fd = server->wake_rd;
        pfds[0].events = POLLIN;
        pfds[1].fd = server->listen_fd;
        pfds[1].events = POLLIN;
        for (size_t i = 0; i < server->nclients; i++) {
            pfds[i + 2].fd = server->clients[i].fd;
            pfds[i + 2].events = POLLIN;
        }
        pthread_mutex_unlock(&server->lock);

        do {
            prc = poll(pfds, nfds, -1);
        } while (prc < 0 && errno == EINTR);

        if (prc < 0) {
            free(pfds);
            return LIPC_IO_ERROR;
        }

        if ((pfds[0].revents & POLLIN) != 0) {
            lipc_server_drain_wake_fd(wake_rd);
        }
        if (lipc_server_is_stopped(server)) {
            free(pfds);
            return LIPC_OK;
        }

        if ((pfds[1].revents & (POLLIN | POLLERR | POLLHUP)) != 0) {
            pthread_mutex_lock(&server->lock);
            run_st = lipc_server_accept_pending(server);
            pthread_mutex_unlock(&server->lock);
            if (run_st != LIPC_OK) {
                free(pfds);
                return run_st;
            }
        }

        for (size_t i = 2; i < nfds; i++) {
            int fd = pfds[i].fd;
            short revents = pfds[i].revents;
            size_t idx = 0;
            if (fd < 0 || revents == 0) {
                continue;
            }
            pthread_mutex_lock(&server->lock);
            if (!lipc_server_find_client_index(server, fd, &idx)) {
                pthread_mutex_unlock(&server->lock);
                continue;
            }
            if ((revents & (POLLHUP | POLLERR | POLLNVAL)) != 0) {
                lipc_server_remove_client(server, idx);
                pthread_mutex_unlock(&server->lock);
                continue;
            }
            if ((revents & POLLIN) == 0) {
                pthread_mutex_unlock(&server->lock);
                continue;
            }
            pthread_mutex_unlock(&server->lock);

            run_st = lipc_server_drain_client_frames(server, idx);
            if (run_st == LIPC_OK) {
                continue;
            }
            if (run_st == LIPC_IO_ERROR || run_st == LIPC_BAD_HEADER || run_st == LIPC_BAD_MAGIC
                || run_st == LIPC_BAD_VERSION || run_st == LIPC_BAD_LENGTH
                || run_st == LIPC_BAD_CHECKSUM) {
                pthread_mutex_lock(&server->lock);
                lipc_server_remove_client(server, idx);
                pthread_mutex_unlock(&server->lock);
                continue;
            }
            free(pfds);
            return run_st;
        }

        free(pfds);
    }
}

lipc_status lipc_method_validate(const lipc_method_desc *desc, const lipc_header *h,
    size_t payload_len)
{
    if (!desc || !h) {
        return LIPC_BAD_HEADER;
    }
    if (h->length != payload_len) {
        return LIPC_INVALID_PARAMS;
    }

    switch (desc->index) {
    case LIPC_FIELD_FORBIDDEN:
        if (h->index != 0) {
            return LIPC_INVALID_PARAMS;
        }
        break;
    case LIPC_FIELD_REQUIRED:
        if (h->index == 0) {
            return LIPC_INVALID_PARAMS;
        }
        break;
    case LIPC_FIELD_OPTIONAL:
    default:
        break;
    }

    switch (desc->value) {
    case LIPC_FIELD_FORBIDDEN:
        if (h->value != 0) {
            return LIPC_INVALID_PARAMS;
        }
        break;
    case LIPC_FIELD_REQUIRED:
        if (h->value == 0) {
            return LIPC_INVALID_PARAMS;
        }
        break;
    case LIPC_FIELD_OPTIONAL:
    default:
        break;
    }

    switch (desc->payload) {
    case LIPC_FIELD_FORBIDDEN:
        if (payload_len != 0) {
            return LIPC_INVALID_PARAMS;
        }
        break;
    case LIPC_FIELD_OPTIONAL:
    case LIPC_FIELD_REQUIRED:
        if (payload_len < desc->payload_min_len || payload_len > desc->payload_max_len) {
            return LIPC_INVALID_PARAMS;
        }
        break;
    default:
        return LIPC_INVALID_PARAMS;
    }

    return LIPC_OK;
}

lipc_status lipc_server_reply(lipc_server_reply_ctx *reply, uint16_t wire_status, uint16_t index,
    uint64_t value, const void *payload, size_t payload_len)
{
    if (!reply || reply->fd < 0) {
        return LIPC_BAD_HEADER;
    }
    if (payload_len > 0 && !payload) {
        return LIPC_BAD_HEADER;
    }

    lipc_header out = {
        .request = reply->request_hdr.request,
        .status = wire_status,
        .index = index,
        .tid = reply->request_hdr.tid,
        .length = (uint32_t)payload_len,
        .value = value,
    };
    if (out.length != payload_len) {
        return LIPC_BAD_LENGTH;
    }
    return lipc_frame_write(reply->fd, &out, payload, payload_len);
}

const lipc_server_handler *lipc_server_handler_lookup(const lipc_server_handler *handlers,
    size_t nhandlers, uint16_t request_id)
{
    if (!handlers || nhandlers == 0) {
        return NULL;
    }
    for (size_t i = 0; i < nhandlers; i++) {
        if (handlers[i].request_id == request_id) {
            return &handlers[i];
        }
    }
    return NULL;
}

lipc_status lipc_server_dispatch(const lipc_server_handler *handlers, size_t nhandlers, void *user,
    int fd, const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    if (!req_hdr || (payload_len > 0 && !payload)) {
        return LIPC_BAD_HEADER;
    }
    if (req_hdr->tid == 0) {
        return LIPC_INVALID_PARAMS;
    }

    const lipc_server_handler *h = lipc_server_handler_lookup(handlers, nhandlers, req_hdr->request);
    if (!h || !h->request_desc || !h->fn) {
        return LIPC_UNKNOWN_REQUEST;
    }

    lipc_status v = lipc_method_validate(h->request_desc, req_hdr, payload_len);
    if (v != LIPC_OK) {
        return v;
    }

    lipc_server_reply_ctx ctx = {
        .fd = fd,
    };
    ctx.request_hdr = *req_hdr;

    return h->fn(user, &ctx, req_hdr, payload, payload_len);
}

const lipc_client_reply_handler *lipc_client_reply_lookup(const lipc_client_reply_handler *handlers,
    size_t nhandlers, uint16_t request_id)
{
    if (!handlers || nhandlers == 0) {
        return NULL;
    }
    for (size_t i = 0; i < nhandlers; i++) {
        if (handlers[i].request_id == request_id) {
            return &handlers[i];
        }
    }
    return NULL;
}

const lipc_client_notify_handler *lipc_client_notify_lookup(const lipc_client_notify_handler *handlers,
    size_t nhandlers, uint16_t request_id)
{
    if (!handlers || nhandlers == 0) {
        return NULL;
    }
    for (size_t i = 0; i < nhandlers; i++) {
        if (handlers[i].request_id == request_id) {
            return &handlers[i];
        }
    }
    return NULL;
}

lipc_status lipc_client_dispatch_incoming(const lipc_client_route_table *routes, void *user,
    const lipc_header *hdr, const uint8_t *payload, size_t payload_len)
{
    if (!routes || !hdr) {
        return LIPC_BAD_HEADER;
    }
    if (payload_len > 0 && !payload) {
        return LIPC_BAD_HEADER;
    }

    if (hdr->tid == 0) {
        const lipc_client_notify_handler *nh
            = lipc_client_notify_lookup(routes->notify_handlers, routes->n_notify, hdr->request);
        if (!nh || !nh->notify_desc || !nh->on_notify) {
            return LIPC_UNKNOWN_REQUEST;
        }
        lipc_status v = lipc_method_validate(nh->notify_desc, hdr, payload_len);
        if (v != LIPC_OK) {
            return v;
        }
        return nh->on_notify(user, hdr, payload, payload_len);
    }

    const lipc_client_reply_handler *rh
        = lipc_client_reply_lookup(routes->reply_handlers, routes->n_reply, hdr->request);
    if (!rh || !rh->reply_desc || !rh->on_reply) {
        return LIPC_UNKNOWN_REQUEST;
    }
    lipc_status v = lipc_method_validate(rh->reply_desc, hdr, payload_len);
    if (v != LIPC_OK) {
        return v;
    }
    return rh->on_reply(user, hdr, payload, payload_len);
}

static int lipc_client_inflight_has_tid(const lipc_client *client, uint32_t tid)
{
    for (const lipc_client_inflight *it = client->inflight; it; it = it->next) {
        if (it->tid == tid) {
            return 1;
        }
    }
    return 0;
}

static lipc_status lipc_client_next_tid_locked(lipc_client *client, uint32_t *out_tid)
{
    uint64_t attempts = 0;
    uint32_t candidate = client->next_tid;

    while (attempts < 0xffffffffull) {
        candidate++;
        if (candidate == 0) {
            candidate = 1;
        }
        if (!lipc_client_inflight_has_tid(client, candidate)) {
            client->next_tid = candidate;
            *out_tid = candidate;
            return LIPC_OK;
        }
        attempts++;
    }
    return LIPC_INTERNAL_ERROR;
}

static void lipc_client_inflight_free_list(lipc_client_inflight *head)
{
    while (head) {
        lipc_client_inflight *next = head->next;
        free(head);
        head = next;
    }
}

static lipc_client_inflight *lipc_client_inflight_pop_tid(lipc_client *client, uint32_t tid)
{
    lipc_client_inflight *prev = NULL;
    lipc_client_inflight *cur = client->inflight;

    while (cur) {
        if (cur->tid == tid) {
            if (prev) {
                prev->next = cur->next;
            } else {
                client->inflight = cur->next;
            }
            cur->next = NULL;
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}

lipc_client *lipc_client_create(uint32_t max_payload, const lipc_client_route_table *routes,
    void *route_user)
{
    lipc_client *client = calloc(1, sizeof *client);
    if (!client) {
        return NULL;
    }

    if (pthread_mutex_init(&client->lock, NULL) != 0) {
        free(client);
        return NULL;
    }
    if (pthread_mutex_init(&client->io_lock, NULL) != 0) {
        pthread_mutex_destroy(&client->lock);
        free(client);
        return NULL;
    }
    if (pthread_rwlock_init(&client->routes_lock, NULL) != 0) {
        pthread_mutex_destroy(&client->io_lock);
        pthread_mutex_destroy(&client->lock);
        free(client);
        return NULL;
    }

    client->max_payload = max_payload ? max_payload : LIPC_MAX_PAYLOAD_DEFAULT;
    client->route_user = route_user;
    client->next_tid = 0;
    lipc_frame_reader_init(&client->reader);

    if (client->max_payload > 0) {
        client->rx_payload = malloc(client->max_payload);
        if (!client->rx_payload) {
            pthread_rwlock_destroy(&client->routes_lock);
            pthread_mutex_destroy(&client->io_lock);
            pthread_mutex_destroy(&client->lock);
            free(client);
            return NULL;
        }
    }

    if (routes) {
        if (routes->reply_handlers && routes->n_reply > 0) {
            client->reply_handlers = calloc(routes->n_reply, sizeof *client->reply_handlers);
            if (!client->reply_handlers) {
                free(client->rx_payload);
                pthread_rwlock_destroy(&client->routes_lock);
                pthread_mutex_destroy(&client->io_lock);
                pthread_mutex_destroy(&client->lock);
                free(client);
                return NULL;
            }
            memcpy(client->reply_handlers, routes->reply_handlers,
                routes->n_reply * sizeof *routes->reply_handlers);
            client->routes.reply_handlers = client->reply_handlers;
            client->routes.n_reply = routes->n_reply;
        }
        if (routes->notify_handlers && routes->n_notify > 0) {
            client->notify_handlers = calloc(routes->n_notify, sizeof *client->notify_handlers);
            if (!client->notify_handlers) {
                free(client->reply_handlers);
                free(client->rx_payload);
                pthread_rwlock_destroy(&client->routes_lock);
                pthread_mutex_destroy(&client->io_lock);
                pthread_mutex_destroy(&client->lock);
                free(client);
                return NULL;
            }
            memcpy(client->notify_handlers, routes->notify_handlers,
                routes->n_notify * sizeof *routes->notify_handlers);
            client->routes.notify_handlers = client->notify_handlers;
            client->routes.n_notify = routes->n_notify;
        }
    }

    return client;
}

void lipc_client_destroy(lipc_client *client)
{
    lipc_client_inflight *head;

    if (!client) {
        return;
    }

    pthread_mutex_lock(&client->lock);
    head = client->inflight;
    client->inflight = NULL;
    pthread_mutex_unlock(&client->lock);

    lipc_client_inflight_free_list(head);

    pthread_rwlock_wrlock(&client->routes_lock);
    free(client->reply_handlers);
    free(client->notify_handlers);
    client->reply_handlers = NULL;
    client->notify_handlers = NULL;
    memset(&client->routes, 0, sizeof client->routes);
    client->route_user = NULL;
    pthread_rwlock_unlock(&client->routes_lock);

    free(client->rx_payload);
    pthread_rwlock_destroy(&client->routes_lock);
    pthread_mutex_destroy(&client->io_lock);
    pthread_mutex_destroy(&client->lock);
    free(client);
}

lipc_status lipc_client_next_tid(lipc_client *client, uint32_t *out_tid)
{
    lipc_status st;

    if (!client || !out_tid) {
        return LIPC_BAD_HEADER;
    }

    pthread_mutex_lock(&client->lock);
    st = lipc_client_next_tid_locked(client, out_tid);
    pthread_mutex_unlock(&client->lock);
    return st;
}

lipc_status lipc_client_request_async(lipc_client *client, int fd, uint16_t request_id,
    uint16_t index, uint64_t value, const void *payload, size_t payload_len,
    lipc_client_completion_cb on_complete, void *complete_user, uint32_t *out_tid)
{
    lipc_status st;
    uint32_t tid = 0;
    lipc_client_inflight *entry = NULL;
    lipc_header req = { 0 };

    if (!client || fd < 0 || (payload_len > 0 && !payload)) {
        return LIPC_BAD_HEADER;
    }
    if ((uint32_t)payload_len != payload_len) {
        return LIPC_BAD_LENGTH;
    }

    entry = calloc(1, sizeof *entry);
    if (!entry) {
        return LIPC_INTERNAL_ERROR;
    }
    entry->request_id = request_id;
    entry->on_complete = on_complete;
    entry->complete_user = complete_user;

    pthread_mutex_lock(&client->lock);
    st = lipc_client_next_tid_locked(client, &tid);
    if (st == LIPC_OK) {
        entry->tid = tid;
        entry->next = client->inflight;
        client->inflight = entry;
        entry = NULL;
    }
    pthread_mutex_unlock(&client->lock);

    if (st != LIPC_OK) {
        free(entry);
        return st;
    }

    req.request = request_id;
    req.status = LIPC_OK;
    req.index = index;
    req.tid = tid;
    req.length = (uint32_t)payload_len;
    req.value = value;

    st = lipc_frame_write(fd, &req, payload, payload_len);
    if (st != LIPC_OK) {
        pthread_mutex_lock(&client->lock);
        entry = lipc_client_inflight_pop_tid(client, tid);
        pthread_mutex_unlock(&client->lock);
        free(entry);
        return st;
    }

    if (out_tid) {
        *out_tid = tid;
    }
    return LIPC_OK;
}

lipc_status lipc_client_dispatch_frame(lipc_client *client, const lipc_header *hdr,
    const uint8_t *payload, size_t payload_len)
{
    lipc_client_inflight *entry = NULL;
    lipc_client_route_table routes = { 0 };
    void *route_user = NULL;

    if (!client || !hdr || (payload_len > 0 && !payload)) {
        return LIPC_BAD_HEADER;
    }

    if (hdr->tid == 0) {
        pthread_rwlock_rdlock(&client->routes_lock);
        routes = client->routes;
        route_user = client->route_user;
        pthread_rwlock_unlock(&client->routes_lock);
        if (!routes.notify_handlers && !routes.reply_handlers) {
            return LIPC_UNKNOWN_REQUEST;
        }
        return lipc_client_dispatch_incoming(&routes, route_user, hdr, payload,
            payload_len);
    }

    pthread_mutex_lock(&client->lock);
    entry = lipc_client_inflight_pop_tid(client, hdr->tid);
    pthread_mutex_unlock(&client->lock);
    if (!entry) {
        return LIPC_NOT_FOUND;
    }

    if (entry->request_id != hdr->request) {
        free(entry);
        return LIPC_INVALID_PARAMS;
    }

    if (entry->on_complete) {
        lipc_status st = entry->on_complete(entry->complete_user, entry->tid, hdr, payload,
            payload_len);
        free(entry);
        return st;
    }

    pthread_rwlock_rdlock(&client->routes_lock);
    routes = client->routes;
    route_user = client->route_user;
    pthread_rwlock_unlock(&client->routes_lock);
    if (routes.reply_handlers || routes.notify_handlers) {
        lipc_status st = lipc_client_dispatch_incoming(&routes, route_user, hdr,
            payload, payload_len);
        free(entry);
        return st;
    }

    free(entry);
    return LIPC_OK;
}

lipc_status lipc_client_poll_once(lipc_client *client, int fd)
{
    uint8_t scratch[4096];
    uint8_t wire[LIPC_HEADER_SIZE];
    lipc_header hdr;
    size_t payload_len = 0;
    lipc_status st;

    if (!client || fd < 0 || (client->max_payload > 0 && !client->rx_payload)) {
        return LIPC_BAD_HEADER;
    }

    pthread_mutex_lock(&client->io_lock);
    st = lipc_frame_reader_read(fd, &client->reader, client->max_payload, scratch, sizeof scratch,
        wire, &hdr, client->rx_payload, client->max_payload, &payload_len);
    pthread_mutex_unlock(&client->io_lock);
    if (st != LIPC_OK) {
        return st;
    }

    return lipc_client_dispatch_frame(client, &hdr, client->rx_payload, payload_len);
}

typedef struct lipc_sync_wait {
    lipc_header *reply_hdr;
    uint8_t *reply_payload;
    size_t reply_cap;
    size_t *reply_len;
    int done;
} lipc_sync_wait;

static lipc_status lipc_client_sync_complete(void *user, uint32_t tid, const lipc_header *hdr,
    const uint8_t *payload, size_t payload_len)
{
    lipc_sync_wait *wait = user;
    (void)tid;

    if (!wait || !hdr || (payload_len > 0 && !payload)) {
        return LIPC_BAD_HEADER;
    }

    if (payload_len > wait->reply_cap) {
        return LIPC_BAD_LENGTH;
    }

    if (wait->reply_hdr) {
        *wait->reply_hdr = *hdr;
    }
    if (payload_len > 0 && wait->reply_payload) {
        memcpy(wait->reply_payload, payload, payload_len);
    }
    if (wait->reply_len) {
        *wait->reply_len = payload_len;
    }

    wait->done = 1;
    return LIPC_OK;
}

lipc_status lipc_client_call(lipc_client *client, int fd, uint16_t request_id, uint16_t index,
    uint64_t value, const void *payload, size_t payload_len, lipc_header *reply_hdr,
    void *reply_payload, size_t reply_payload_cap, size_t *reply_len)
{
    lipc_sync_wait wait;
    uint32_t tid = 0;
    lipc_status st;

    if (!client || fd < 0 || (payload_len > 0 && !payload)
        || (reply_payload_cap > 0 && !reply_payload)) {
        return LIPC_BAD_HEADER;
    }

    memset(&wait, 0, sizeof wait);
    wait.reply_hdr = reply_hdr;
    wait.reply_payload = reply_payload;
    wait.reply_cap = reply_payload_cap;
    wait.reply_len = reply_len;

    st = lipc_client_request_async(client, fd, request_id, index, value, payload, payload_len,
        lipc_client_sync_complete, &wait, &tid);
    if (st != LIPC_OK) {
        return st;
    }

    while (!wait.done) {
        st = lipc_client_poll_once(client, fd);
        if (st == LIPC_WOULD_BLOCK) {
            struct pollfd pfd = {
                .fd = fd,
                .events = POLLIN,
            };
            int prc;
            do {
                prc = poll(&pfd, 1, -1);
            } while (prc < 0 && errno == EINTR);
            if (prc < 0) {
                return LIPC_IO_ERROR;
            }
            continue;
        }
        if (st != LIPC_OK) {
            pthread_mutex_lock(&client->lock);
            lipc_client_inflight *entry = lipc_client_inflight_pop_tid(client, tid);
            pthread_mutex_unlock(&client->lock);
            free(entry);
            return st;
        }
    }

    return LIPC_OK;
}
