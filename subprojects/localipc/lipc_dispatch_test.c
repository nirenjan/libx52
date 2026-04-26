/*
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <localipc/lipc.h>

#include <stdarg.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <setjmp.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cmocka.h>

static void test_method_validate_index_forbidden(void **state)
{
    (void)state;
    const lipc_method_desc d = {
        .index = LIPC_FIELD_FORBIDDEN,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_FORBIDDEN,
        .payload_min_len = 0,
        .payload_max_len = 0,
    };
    lipc_header h = { .index = 1, .length = 0 };
    assert_int_equal(lipc_method_validate(&d, &h, 0), LIPC_INVALID_PARAMS);
    h.index = 0;
    assert_int_equal(lipc_method_validate(&d, &h, 0), LIPC_OK);
}

static void test_method_validate_payload_bounds(void **state)
{
    (void)state;
    const lipc_method_desc d = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_OPTIONAL,
        .payload_min_len = 2,
        .payload_max_len = 4,
    };
    lipc_header h = { .length = 1 };
    assert_int_equal(lipc_method_validate(&d, &h, 1), LIPC_INVALID_PARAMS);
    h.length = 2;
    assert_int_equal(lipc_method_validate(&d, &h, 2), LIPC_OK);
    h.length = 5;
    assert_int_equal(lipc_method_validate(&d, &h, 5), LIPC_INVALID_PARAMS);
}

static int g_server_cb_calls;

static lipc_status server_echo_fn(void *user, lipc_server_reply_ctx *reply, const lipc_header *req_hdr,
    const uint8_t *payload, size_t payload_len)
{
    (void)user;
    (void)req_hdr;
    g_server_cb_calls++;
    return lipc_server_reply(reply, LIPC_OK, 0, 0, payload, payload_len);
}

static void test_server_dispatch_reply_echoes_tid(void **state)
{
    (void)state;
    int sp[2];

    assert_int_equal(socketpair(AF_UNIX, SOCK_STREAM, 0, sp), 0);

    const lipc_method_desc desc = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_OPTIONAL,
        .payload_min_len = 0,
        .payload_max_len = 64,
    };
    const lipc_server_handler handlers[] = {
        { .request_id = 100, .request_desc = &desc, .fn = server_echo_fn },
    };

    lipc_header req = {
        .request = 100,
        .status = 0,
        .index = 0,
        .tid = 0xdeadbeefu,
        .length = 3,
        .value = 0,
    };
    const uint8_t pl_in[] = { 'a', 'b', 'c' };

    g_server_cb_calls = 0;
    assert_int_equal(lipc_frame_write(sp[1], &req, pl_in, sizeof pl_in), LIPC_OK);

    uint8_t wire[LIPC_HEADER_SIZE];
    lipc_header rh;
    uint8_t pl_out[16];
    size_t plen = 0;

    assert_int_equal(
        lipc_frame_read(sp[0], LIPC_MAX_PAYLOAD_DEFAULT, wire, &rh, pl_out, sizeof pl_out, &plen),
        LIPC_OK);
    assert_int_equal((int)plen, 3);

    assert_int_equal(lipc_server_dispatch(handlers, 1, NULL, sp[0], &rh, pl_out, plen), LIPC_OK);
    assert_int_equal(g_server_cb_calls, 1);

    assert_int_equal(
        lipc_frame_read(sp[1], LIPC_MAX_PAYLOAD_DEFAULT, wire, &rh, pl_out, sizeof pl_out, &plen),
        LIPC_OK);
    assert_int_equal(rh.tid, 0xdeadbeefu);
    assert_int_equal(rh.request, 100);
    assert_int_equal(rh.status, LIPC_OK);
    assert_int_equal((int)plen, 3);
    assert_memory_equal(pl_out, pl_in, 3);

    close(sp[0]);
    close(sp[1]);
}

static void test_server_dispatch_rejects_tid_zero(void **state)
{
    (void)state;
    const lipc_method_desc desc = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_FORBIDDEN,
        .payload_min_len = 0,
        .payload_max_len = 0,
    };
    const lipc_server_handler handlers[] = {
        { .request_id = 1, .request_desc = &desc, .fn = server_echo_fn },
    };
    lipc_header h = { .request = 1, .tid = 0, .length = 0 };

    g_server_cb_calls = 0;
    assert_int_equal(lipc_server_dispatch(handlers, 1, NULL, -1, &h, NULL, 0), LIPC_INVALID_PARAMS);
    assert_int_equal(g_server_cb_calls, 0);
}

static int g_notify_calls;
static int g_reply_calls;

static lipc_status notify_fn(void *user, const lipc_header *hdr, const uint8_t *payload,
    size_t payload_len)
{
    (void)user;
    (void)payload;
    (void)payload_len;
    assert_int_equal(hdr->tid, 0u);
    g_notify_calls++;
    return LIPC_OK;
}

static lipc_status reply_fn(void *user, const lipc_header *hdr, const uint8_t *payload,
    size_t payload_len)
{
    (void)user;
    (void)payload;
    (void)payload_len;
    assert_int_equal(hdr->tid, 42u);
    g_reply_calls++;
    return LIPC_OK;
}

static void test_client_dispatch_notify_vs_reply(void **state)
{
    (void)state;
    const lipc_method_desc nd = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_FORBIDDEN,
        .payload_min_len = 0,
        .payload_max_len = 0,
    };
    const lipc_method_desc rd = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_FORBIDDEN,
        .payload_min_len = 0,
        .payload_max_len = 0,
    };
    const lipc_client_notify_handler nh[] = {
        { .request_id = 7, .notify_desc = &nd, .on_notify = notify_fn },
    };
    const lipc_client_reply_handler rh[] = {
        { .request_id = 8, .reply_desc = &rd, .on_reply = reply_fn },
    };
    const lipc_client_route_table tab = {
        .reply_handlers = rh,
        .n_reply = 1,
        .notify_handlers = nh,
        .n_notify = 1,
    };

    g_notify_calls = 0;
    g_reply_calls = 0;

    lipc_header nhdr = { .request = 7, .tid = 0, .length = 0 };
    assert_int_equal(lipc_client_dispatch_incoming(&tab, NULL, &nhdr, NULL, 0), LIPC_OK);
    assert_int_equal(g_notify_calls, 1);
    assert_int_equal(g_reply_calls, 0);

    lipc_header rhdr = { .request = 8, .tid = 42, .length = 0 };
    assert_int_equal(lipc_client_dispatch_incoming(&tab, NULL, &rhdr, NULL, 0), LIPC_OK);
    assert_int_equal(g_notify_calls, 1);
    assert_int_equal(g_reply_calls, 1);

    lipc_header unk = { .request = 999, .tid = 0, .length = 0 };
    assert_int_equal(lipc_client_dispatch_incoming(&tab, NULL, &unk, NULL, 0), LIPC_UNKNOWN_REQUEST);
}

static void test_handler_lookup(void **state)
{
    (void)state;
    const lipc_method_desc d = { 0 };
    const lipc_server_handler handlers[] = {
        { .request_id = 10, .request_desc = &d, .fn = NULL },
        { .request_id = 20, .request_desc = &d, .fn = NULL },
    };
    assert_non_null(lipc_server_handler_lookup(handlers, 2, 10));
    assert_non_null(lipc_server_handler_lookup(handlers, 2, 20));
    assert_null(lipc_server_handler_lookup(handlers, 2, 99));
    assert_int_equal(lipc_server_handler_lookup(handlers, 2, 10)->request_id, 10);
}

static void test_client_next_tid_nonzero_unique(void **state)
{
    (void)state;
    lipc_client *client = lipc_client_create(0, NULL, NULL);
    uint32_t tid1 = 0;
    uint32_t tid2 = 0;

    assert_non_null(client);
    assert_int_equal(lipc_client_next_tid(client, &tid1), LIPC_OK);
    assert_int_equal(lipc_client_next_tid(client, &tid2), LIPC_OK);
    assert_true(tid1 != 0);
    assert_true(tid2 != 0);
    assert_true(tid1 != tid2);
    lipc_client_destroy(client);
}

typedef struct tid_thread_args {
    lipc_client *client;
    uint32_t *tids;
    size_t count;
    int status;
} tid_thread_args;

static void *tid_alloc_thread(void *arg)
{
    tid_thread_args *a = arg;

    for (size_t i = 0; i < a->count; i++) {
        if (lipc_client_next_tid(a->client, &a->tids[i]) != LIPC_OK || a->tids[i] == 0) {
            a->status = -1;
            return NULL;
        }
    }
    a->status = 0;
    return NULL;
}

static int uint32_cmp(const void *lhs, const void *rhs)
{
    const uint32_t a = *(const uint32_t *)lhs;
    const uint32_t b = *(const uint32_t *)rhs;
    if (a < b) {
        return -1;
    }
    if (a > b) {
        return 1;
    }
    return 0;
}

static void test_client_next_tid_thread_safe(void **state)
{
    (void)state;
    enum {
        THREADS = 4,
        PER_THREAD = 128,
        TOTAL = THREADS * PER_THREAD,
    };
    lipc_client *client = lipc_client_create(0, NULL, NULL);
    pthread_t threads[THREADS];
    tid_thread_args args[THREADS];
    uint32_t tids[TOTAL];

    assert_non_null(client);
    for (size_t i = 0; i < THREADS; i++) {
        args[i].client = client;
        args[i].tids = &tids[i * PER_THREAD];
        args[i].count = PER_THREAD;
        args[i].status = -1;
        assert_int_equal(pthread_create(&threads[i], NULL, tid_alloc_thread, &args[i]), 0);
    }
    for (size_t i = 0; i < THREADS; i++) {
        assert_int_equal(pthread_join(threads[i], NULL), 0);
        assert_int_equal(args[i].status, 0);
    }

    qsort(tids, TOTAL, sizeof tids[0], uint32_cmp);
    for (size_t i = 1; i < TOTAL; i++) {
        assert_true(tids[i - 1] != tids[i]);
    }

    lipc_client_destroy(client);
}

static int g_complete_calls;
static uint32_t g_complete_tid;

static lipc_status completion_fn(void *user, uint32_t tid, const lipc_header *hdr,
    const uint8_t *payload, size_t payload_len)
{
    (void)user;
    assert_non_null(hdr);
    assert_non_null(payload);
    assert_int_equal(payload_len, 2);
    assert_int_equal(hdr->status, LIPC_OK);
    assert_int_equal(hdr->tid, tid);
    assert_memory_equal(payload, "ok", 2);
    g_complete_tid = tid;
    g_complete_calls++;
    return LIPC_OK;
}

static void test_client_request_async_and_poll(void **state)
{
    (void)state;
    int sp[2];
    lipc_client *client;
    uint32_t tid = 0;
    uint8_t wire[LIPC_HEADER_SIZE];
    lipc_header req;
    uint8_t req_payload[8];
    size_t req_len = 0;

    assert_int_equal(socketpair(AF_UNIX, SOCK_STREAM, 0, sp), 0);
    client = lipc_client_create(0, NULL, NULL);
    assert_non_null(client);

    g_complete_calls = 0;
    g_complete_tid = 0;
    assert_int_equal(lipc_client_request_async(client, sp[0], 77, 3, 9, "hi", 2, completion_fn,
                          NULL, &tid),
        LIPC_OK);
    assert_true(tid != 0);

    assert_int_equal(
        lipc_frame_read(sp[1], LIPC_MAX_PAYLOAD_DEFAULT, wire, &req, req_payload, sizeof req_payload,
            &req_len),
        LIPC_OK);
    assert_int_equal(req.request, 77);
    assert_int_equal(req.tid, tid);
    assert_int_equal(req_len, 2);

    lipc_header reply = {
        .request = req.request,
        .status = LIPC_OK,
        .index = req.index,
        .tid = req.tid,
        .length = 2,
        .value = 0,
    };
    assert_int_equal(lipc_frame_write(sp[1], &reply, "ok", 2), LIPC_OK);
    assert_int_equal(lipc_client_poll_once(client, sp[0]), LIPC_OK);
    assert_int_equal(g_complete_calls, 1);
    assert_int_equal(g_complete_tid, tid);

    lipc_client_destroy(client);
    close(sp[0]);
    close(sp[1]);
}

typedef struct sync_server_args {
    int fd;
} sync_server_args;

static void *sync_server_thread(void *arg)
{
    sync_server_args *a = arg;
    uint8_t wire[LIPC_HEADER_SIZE];
    lipc_header req;
    uint8_t req_payload[16];
    size_t req_len = 0;

    if (lipc_frame_read(a->fd, LIPC_MAX_PAYLOAD_DEFAULT, wire, &req, req_payload, sizeof req_payload,
            &req_len)
        != LIPC_OK) {
        return NULL;
    }

    lipc_header notify = {
        .request = 500,
        .status = LIPC_OK,
        .index = 0,
        .tid = 0,
        .length = 0,
        .value = 0,
    };
    (void)lipc_frame_write(a->fd, &notify, NULL, 0);

    lipc_header reply = {
        .request = req.request,
        .status = LIPC_OK,
        .index = req.index,
        .tid = req.tid,
        .length = 4,
        .value = 0x55ull,
    };
    (void)lipc_frame_write(a->fd, &reply, "pong", 4);
    return NULL;
}

static int g_notify_seen;

static lipc_status sync_notify_fn(void *user, const lipc_header *hdr, const uint8_t *payload,
    size_t payload_len)
{
    (void)user;
    (void)payload;
    (void)payload_len;
    assert_non_null(hdr);
    assert_int_equal(hdr->tid, 0u);
    assert_int_equal(hdr->request, 500u);
    g_notify_seen++;
    return LIPC_OK;
}

static void test_client_call_sync_with_notify_dispatch(void **state)
{
    (void)state;
    int sp[2];
    pthread_t thr;
    sync_server_args args;
    const lipc_method_desc notify_desc = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_FORBIDDEN,
        .payload_min_len = 0,
        .payload_max_len = 0,
    };
    const lipc_client_notify_handler nh[] = {
        { .request_id = 500, .notify_desc = &notify_desc, .on_notify = sync_notify_fn },
    };
    const lipc_client_route_table routes = {
        .reply_handlers = NULL,
        .n_reply = 0,
        .notify_handlers = nh,
        .n_notify = 1,
    };
    lipc_client *client;
    lipc_header reply_hdr;
    uint8_t reply_payload[16];
    size_t reply_len = 0;

    assert_int_equal(socketpair(AF_UNIX, SOCK_STREAM, 0, sp), 0);
    client = lipc_client_create(0, &routes, NULL);
    assert_non_null(client);

    g_notify_seen = 0;
    args.fd = sp[1];
    assert_int_equal(pthread_create(&thr, NULL, sync_server_thread, &args), 0);

    assert_int_equal(lipc_client_call(client, sp[0], 501, 1, 0, "ping", 4, &reply_hdr, reply_payload,
                          sizeof reply_payload, &reply_len),
        LIPC_OK);
    assert_int_equal((int)reply_len, 4);
    assert_memory_equal(reply_payload, "pong", 4);
    assert_int_equal(reply_hdr.request, 501);
    assert_int_equal(reply_hdr.tid == 0, 0);
    assert_int_equal(g_notify_seen, 1);

    assert_int_equal(pthread_join(thr, NULL), 0);
    lipc_client_destroy(client);
    close(sp[0]);
    close(sp[1]);
}

typedef struct server_thread_args {
    lipc_server *server;
    lipc_status run_status;
} server_thread_args;

static void *server_run_thread(void *arg)
{
    server_thread_args *a = arg;

    a->run_status = lipc_server_run(a->server);
    return NULL;
}

static int make_listener_socket(char *path, size_t path_cap)
{
    if (!path || path_cap == 0) {
        return -1;
    }
    if (snprintf(path, path_cap, "/tmp/lipc_dispatch_%ld_%ld.sock", (long)getpid(), random())
        >= (int)path_cap) {
        return -1;
    }
    return lipc_socket_listen(path, 8, LIPC_SOCKET_NONBLOCK);
}

static int connect_client_socket(const char *path)
{
    return lipc_socket_connect(path, 0u);
}

static int write_full_buf(int fd, const uint8_t *buf, size_t len)
{
    size_t off = 0;

    while (off < len) {
        ssize_t n = write(fd, buf + off, len - off);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            return -1;
        }
        off += (size_t)n;
    }
    return 0;
}

static void test_server_run_accept_dispatch_stop(void **state)
{
    (void)state;
    char sock_path[108];
    int listen_fd;
    int client_fd = -1;
    pthread_t thr;
    server_thread_args args;
    lipc_server *server;
    const lipc_method_desc desc = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_OPTIONAL,
        .payload_min_len = 0,
        .payload_max_len = 64,
    };
    const lipc_server_handler handlers[] = {
        { .request_id = 100, .request_desc = &desc, .fn = server_echo_fn },
    };
    lipc_header req = {
        .request = 100,
        .status = LIPC_OK,
        .index = 1,
        .tid = 55,
        .length = 5,
        .value = 7,
    };
    uint8_t wire[LIPC_HEADER_SIZE];
    uint8_t out_payload[16];
    lipc_header out_hdr;
    size_t out_len = 0;

    listen_fd = make_listener_socket(sock_path, sizeof sock_path);
    assert_true(listen_fd >= 0);

    server = lipc_server_create(listen_fd, 0, handlers, 1, NULL);
    assert_non_null(server);

    args.server = server;
    args.run_status = LIPC_INTERNAL_ERROR;
    assert_int_equal(pthread_create(&thr, NULL, server_run_thread, &args), 0);

    client_fd = connect_client_socket(sock_path);
    assert_true(client_fd >= 0);

    g_server_cb_calls = 0;
    assert_int_equal(lipc_frame_write(client_fd, &req, "hello", 5), LIPC_OK);
    assert_int_equal(
        lipc_frame_read(client_fd, LIPC_MAX_PAYLOAD_DEFAULT, wire, &out_hdr, out_payload,
            sizeof out_payload, &out_len),
        LIPC_OK);
    assert_int_equal(g_server_cb_calls, 1);
    assert_int_equal(out_hdr.request, 100);
    assert_int_equal(out_hdr.tid, 55);
    assert_int_equal((int)out_len, 5);
    assert_memory_equal(out_payload, "hello", 5);

    assert_int_equal(lipc_server_stop(server), LIPC_OK);
    assert_int_equal(pthread_join(thr, NULL), 0);
    assert_int_equal(args.run_status, LIPC_OK);

    close(client_fd);
    lipc_server_destroy(server);
    unlink(sock_path);
}

static void test_server_run_stream_frames_and_close(void **state)
{
    (void)state;
    char sock_path[108];
    int listen_fd;
    int client_fd = -1;
    pthread_t thr;
    server_thread_args args;
    lipc_server *server;
    const lipc_method_desc desc = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_OPTIONAL,
        .payload_min_len = 0,
        .payload_max_len = 64,
    };
    const lipc_server_handler handlers[] = {
        { .request_id = 100, .request_desc = &desc, .fn = server_echo_fn },
    };
    lipc_header req1 = {
        .request = 100,
        .status = LIPC_OK,
        .index = 0,
        .tid = 101,
        .length = 3,
        .value = 0,
    };
    lipc_header req2 = {
        .request = 100,
        .status = LIPC_OK,
        .index = 0,
        .tid = 102,
        .length = 0,
        .value = 0,
    };
    uint8_t frame1[LIPC_HEADER_SIZE];
    uint8_t frame2[LIPC_HEADER_SIZE];
    uint8_t combo[LIPC_HEADER_SIZE + 3 + LIPC_HEADER_SIZE];
    uint8_t wire[LIPC_HEADER_SIZE];
    uint8_t out_payload[16];
    lipc_header out_hdr;
    size_t out_len = 0;

    listen_fd = make_listener_socket(sock_path, sizeof sock_path);
    assert_true(listen_fd >= 0);
    server = lipc_server_create(listen_fd, 0, handlers, 1, NULL);
    assert_non_null(server);

    args.server = server;
    args.run_status = LIPC_INTERNAL_ERROR;
    assert_int_equal(pthread_create(&thr, NULL, server_run_thread, &args), 0);

    client_fd = connect_client_socket(sock_path);
    assert_true(client_fd >= 0);

    lipc_frame_fill_checksum(frame1, &req1, "one", 3);
    lipc_frame_fill_checksum(frame2, &req2, NULL, 0);
    memcpy(combo, frame1, LIPC_HEADER_SIZE);
    memcpy(combo + LIPC_HEADER_SIZE, "one", 3);
    memcpy(combo + LIPC_HEADER_SIZE + 3, frame2, LIPC_HEADER_SIZE);

    g_server_cb_calls = 0;
    assert_int_equal(write_full_buf(client_fd, combo, sizeof combo), 0);

    assert_int_equal(
        lipc_frame_read(client_fd, LIPC_MAX_PAYLOAD_DEFAULT, wire, &out_hdr, out_payload,
            sizeof out_payload, &out_len),
        LIPC_OK);
    assert_int_equal(out_hdr.tid, 101u);
    assert_int_equal((int)out_len, 3);
    assert_memory_equal(out_payload, "one", 3);

    assert_int_equal(
        lipc_frame_read(client_fd, LIPC_MAX_PAYLOAD_DEFAULT, wire, &out_hdr, out_payload,
            sizeof out_payload, &out_len),
        LIPC_OK);
    assert_int_equal(out_hdr.tid, 102u);
    assert_int_equal((int)out_len, 0);
    assert_int_equal(g_server_cb_calls, 2);

    close(client_fd);
    assert_int_equal(lipc_server_stop(server), LIPC_OK);
    assert_int_equal(pthread_join(thr, NULL), 0);
    assert_int_equal(args.run_status, LIPC_OK);

    lipc_server_destroy(server);
    unlink(sock_path);
}

typedef struct concurrent_rpc_args {
    int iterations;
    int status;
} concurrent_rpc_args;

typedef struct concurrent_rpc_server_args {
    int fd;
    int iterations;
    int status;
} concurrent_rpc_server_args;

static void *concurrent_rpc_server_thread(void *arg)
{
    concurrent_rpc_server_args *a = arg;
    uint8_t wire[LIPC_HEADER_SIZE];
    lipc_header req;
    uint8_t req_payload[16];
    size_t req_len = 0;

    a->status = 0;
    for (int i = 0; i < a->iterations; i++) {
        lipc_header reply = { 0 };
        if (lipc_frame_read(a->fd, LIPC_MAX_PAYLOAD_DEFAULT, wire, &req, req_payload, sizeof req_payload,
                &req_len)
            != LIPC_OK) {
            a->status = -1;
            break;
        }

        reply.request = req.request;
        reply.status = LIPC_OK;
        reply.index = req.index;
        reply.tid = req.tid;
        reply.length = (uint32_t)req_len;
        reply.value = req.value;
        if (lipc_frame_write(a->fd, &reply, req_payload, req_len) != LIPC_OK) {
            a->status = -1;
            break;
        }
    }

    close(a->fd);
    return NULL;
}

static void *concurrent_rpc_client_thread(void *arg)
{
    concurrent_rpc_args *a = arg;
    int sp[2];
    pthread_t server_thr;
    concurrent_rpc_server_args server_args;
    lipc_client *client = NULL;
    uint8_t reply_payload[16];
    size_t reply_len = 0;
    lipc_header reply_hdr;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sp) != 0) {
        a->status = -1;
        return NULL;
    }

    server_args.fd = sp[1];
    server_args.iterations = a->iterations;
    server_args.status = -1;
    if (pthread_create(&server_thr, NULL, concurrent_rpc_server_thread, &server_args) != 0) {
        close(sp[0]);
        close(sp[1]);
        a->status = -1;
        return NULL;
    }

    client = lipc_client_create(0, NULL, NULL);
    if (!client) {
        close(sp[0]);
        pthread_join(server_thr, NULL);
        a->status = -1;
        return NULL;
    }

    a->status = 0;
    for (int i = 0; i < a->iterations; i++) {
        const uint16_t request_id = (uint16_t)(700 + (i % 3));
        const uint16_t index = (uint16_t)(i + 1);
        const uint64_t value = (uint64_t)i * 3u;

        reply_len = 0;
        if (lipc_client_call(client, sp[0], request_id, index, value, "xy", 2, &reply_hdr, reply_payload,
                sizeof reply_payload, &reply_len)
            != LIPC_OK) {
            a->status = -1;
            break;
        }
        if (reply_hdr.request != request_id || reply_hdr.tid == 0 || reply_hdr.index != index
            || reply_hdr.value != value || reply_len != 2 || memcmp(reply_payload, "xy", 2) != 0) {
            a->status = -1;
            break;
        }
    }

    close(sp[0]);
    lipc_client_destroy(client);
    pthread_join(server_thr, NULL);
    if (server_args.status != 0) {
        a->status = -1;
    }
    return NULL;
}

static void test_client_call_concurrent_stress(void **state)
{
    (void)state;
    enum {
        THREADS = 4,
        ITERS_PER_THREAD = 64,
    };
    pthread_t threads[THREADS];
    concurrent_rpc_args args[THREADS];

    for (size_t i = 0; i < THREADS; i++) {
        args[i].iterations = ITERS_PER_THREAD;
        args[i].status = -1;
        assert_int_equal(pthread_create(&threads[i], NULL, concurrent_rpc_client_thread, &args[i]), 0);
    }

    for (size_t i = 0; i < THREADS; i++) {
        assert_int_equal(pthread_join(threads[i], NULL), 0);
        assert_int_equal(args[i].status, 0);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_method_validate_index_forbidden),
        cmocka_unit_test(test_method_validate_payload_bounds),
        cmocka_unit_test(test_server_dispatch_reply_echoes_tid),
        cmocka_unit_test(test_server_dispatch_rejects_tid_zero),
        cmocka_unit_test(test_client_dispatch_notify_vs_reply),
        cmocka_unit_test(test_handler_lookup),
        cmocka_unit_test(test_client_next_tid_nonzero_unique),
        cmocka_unit_test(test_client_next_tid_thread_safe),
        cmocka_unit_test(test_client_request_async_and_poll),
        cmocka_unit_test(test_client_call_sync_with_notify_dispatch),
        cmocka_unit_test(test_client_call_concurrent_stress),
        cmocka_unit_test(test_server_run_accept_dispatch_stop),
        cmocka_unit_test(test_server_run_stream_frames_and_close),
    };
    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
