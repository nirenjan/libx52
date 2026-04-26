#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <localipc/lipc.h>

enum {
    REQ_PING = 1,
    REQ_NOTIFY = 2,
};

static volatile sig_atomic_t g_stop = 0;

struct server_thread_ctx {
    lipc_server *server;
    lipc_status run_status;
};

static void on_signal(int sig)
{
    (void)sig;
    g_stop = 1;
}

static lipc_status on_ping(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len)
{
    static const char notify_text[] = "notify: ping handled";
    lipc_header notify_hdr;
    lipc_status st;

    (void)user;

    memset(&notify_hdr, 0, sizeof(notify_hdr));
    notify_hdr.request = REQ_NOTIFY;
    notify_hdr.status = LIPC_OK;
    notify_hdr.tid = 0;
    notify_hdr.length = (uint32_t)(sizeof(notify_text) - 1u);

    st = lipc_frame_write(reply->fd, &notify_hdr, notify_text, sizeof(notify_text) - 1u);
    if (st != LIPC_OK) {
        return st;
    }

    return lipc_server_reply(reply, LIPC_OK, req_hdr->index, req_hdr->value, payload, payload_len);
}

static void *run_server_thread(void *arg)
{
    struct server_thread_ctx *ctx = (struct server_thread_ctx *)arg;
    ctx->run_status = lipc_server_run(ctx->server);
    return NULL;
}

int main(void)
{
    const char *socket_path = lipc_default_socket_path();
    static const lipc_method_desc ping_desc = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_OPTIONAL,
        .payload_min_len = 0,
        .payload_max_len = 1024,
    };
    static const lipc_server_handler handlers[] = {
        {
            .request_id = REQ_PING,
            .request_desc = &ping_desc,
            .fn = on_ping,
        },
    };

    struct server_thread_ctx thread_ctx;
    pthread_t io_thread;
    int listen_fd;
    int rc;

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    listen_fd = lipc_socket_listen(socket_path, 16, LIPC_SOCKET_NONBLOCK);
    if (listen_fd < 0) {
        perror("lipc_socket_listen");
        return 1;
    }

    thread_ctx.server = lipc_server_create(
        listen_fd, LIPC_MAX_PAYLOAD_DEFAULT, handlers, sizeof(handlers) / sizeof(handlers[0]), NULL);
    if (thread_ctx.server == NULL) {
        perror("lipc_server_create");
        close(listen_fd);
        unlink(socket_path);
        return 1;
    }
    thread_ctx.run_status = LIPC_OK;

    rc = pthread_create(&io_thread, NULL, run_server_thread, &thread_ctx);
    if (rc != 0) {
        fprintf(stderr, "pthread_create failed: %s\n", strerror(rc));
        lipc_server_destroy(thread_ctx.server);
        unlink(socket_path);
        return 1;
    }

    while (!g_stop) {
        sleep(1);
    }

    (void)lipc_server_stop(thread_ctx.server);
    (void)pthread_join(io_thread, NULL);

    if (thread_ctx.run_status != LIPC_OK) {
        fprintf(stderr, "server loop failed: %s\n", lipc_status_str(thread_ctx.run_status));
    }

    lipc_server_destroy(thread_ctx.server);
    unlink(socket_path);
    return thread_ctx.run_status == LIPC_OK ? 0 : 1;
}
