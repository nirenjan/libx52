/*
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <localipc/lipc.h>

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cmocka.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(LIPC_OFF_VALUE + 8u == LIPC_HEADER_SIZE, "wire header size");
#endif

static void test_status_helpers(void **state)
{
    (void)state;

    assert_true(lipc_status_is_ok(LIPC_OK));
    assert_int_equal(lipc_status_is_ok(LIPC_BAD_MAGIC), 0);
    assert_int_equal(lipc_status_is_ok((lipc_status)LIPC_STATUS_APP_BASE), 0);
    assert_int_equal(lipc_status_is_ok((lipc_status)0xffff), 0);

    assert_string_equal(lipc_status_str(LIPC_OK), "OK");
    assert_string_equal(lipc_status_str(LIPC_IO_ERROR), "IO_ERROR");
    assert_string_equal(lipc_status_str((lipc_status)LIPC_STATUS_APP_BASE), "APPLICATION_DEFINED");
    assert_string_equal(lipc_status_str((lipc_status)0xff00), "APPLICATION_DEFINED");
    assert_string_equal(lipc_status_str((lipc_status)99), "UNKNOWN_LIPC_STATUS");
}

static void test_header_round_trip(void **state)
{
    (void)state;
    lipc_header in = {
        .magic = LIPC_PROTOCOL_MAGIC,
        .version = LIPC_PROTOCOL_VERSION,
        .request = 42,
        .status = 0,
        .index = 7,
        .tid = 0x11223344u,
        .length = 0,
        .checksum = 0,
        .value = 0x010203040a0b0c0du,
    };
    uint8_t wire[LIPC_HEADER_SIZE];

    lipc_header_encode(wire, &in);
    lipc_header out;
    assert_int_equal(lipc_header_decode(&out, wire, LIPC_MAX_PAYLOAD_DEFAULT), LIPC_OK);
    assert_int_equal(out.magic, in.magic);
    assert_int_equal(out.version, in.version);
    assert_int_equal(out.request, in.request);
    assert_int_equal(out.status, in.status);
    assert_int_equal(out.index, in.index);
    assert_int_equal(out.tid, in.tid);
    assert_int_equal(out.length, in.length);
    assert_int_equal(out.checksum, 0);
    assert_true(out.value == in.value);
}

static void test_crc32_empty_payload_zlib_compat(void **state)
{
    (void)state;
    lipc_header z = { 0 };
    uint8_t wire[LIPC_HEADER_SIZE];

    lipc_header_encode(wire, &z);
    uint32_t c = lipc_checksum_compute(wire, NULL, 0);
    /* Matches CPython zlib.crc32 on this 32-byte blob (checksum field zero). */
    assert_int_equal((int)c, (int)0x5d29d25du);
}

static void test_crc32_payload_zlib_compat(void **state)
{
    (void)state;
    lipc_header h = {
        .magic = LIPC_PROTOCOL_MAGIC,
        .version = LIPC_PROTOCOL_VERSION,
        .request = 0x1234u,
        .status = 0,
        .index = 2,
        .tid = 0x10203040u,
        .length = 5,
        .checksum = 0,
        .value = 0x0102030405060708ull,
    };
    const char payload[] = "hello";
    uint8_t wire[LIPC_HEADER_SIZE];

    lipc_header_encode(wire, &h);
    /* Matches CPython zlib.crc32 for header bytes + payload bytes. */
    assert_int_equal((int)lipc_checksum_compute(wire, payload, 5), (int)0x4b413dc9u);
}

static void test_header_bad_version(void **state)
{
    (void)state;
    lipc_header z = { 0 };
    uint8_t wire[LIPC_HEADER_SIZE];

    lipc_header_encode(wire, &z);
    wire[LIPC_OFF_VERSION + 1] = 2;
    lipc_header out;
    assert_int_equal(lipc_header_decode(&out, wire, LIPC_MAX_PAYLOAD_DEFAULT), LIPC_BAD_VERSION);
}

static void test_header_length_cap(void **state)
{
    (void)state;
    lipc_header h = {
        .length = 64,
    };
    uint8_t wire[LIPC_HEADER_SIZE];

    lipc_header_encode(wire, &h);
    lipc_header out;
    assert_int_equal(lipc_header_decode(&out, wire, 63), LIPC_BAD_LENGTH);
    assert_int_equal(lipc_header_decode(&out, wire, 64), LIPC_OK);
}

static void test_frame_reader_nonblocking(void **state)
{
    (void)state;
    int sp[2];

    assert_int_equal(socketpair(AF_UNIX, SOCK_STREAM, 0, sp), 0);
    assert_int_equal(fcntl(sp[0], F_SETFL, fcntl(sp[0], F_GETFL) | O_NONBLOCK), 0);

    lipc_header hdr = {
        .magic = LIPC_MAGIC,
        .version = LIPC_PROTOCOL_VERSION,
        .request = 9,
        .status = 0,
        .index = 0,
        .tid = 2,
        .length = 4,
        .checksum = 0,
        .value = 0,
    };
    const char *pl = "data";
    uint8_t frame[LIPC_HEADER_SIZE];

    lipc_frame_fill_checksum(frame, &hdr, pl, 4);

    assert_int_equal(write(sp[1], frame, 10), 10);
    assert_int_equal(write(sp[1], frame + 10, 22), 22);
    assert_int_equal(write(sp[1], pl, 4), 4);

    lipc_frame_reader rr;
    lipc_frame_reader_init(&rr);
    uint8_t scratch[512];
    lipc_header out;
    uint8_t payload[16];
    size_t plen = 0;
    uint8_t out_wire[LIPC_HEADER_SIZE];

    lipc_status st = LIPC_WOULD_BLOCK;
    int spins = 0;

    while (st == LIPC_WOULD_BLOCK && spins < 32) {
        st = lipc_frame_reader_read(sp[0], &rr, LIPC_MAX_PAYLOAD_DEFAULT, scratch, sizeof scratch,
            out_wire, &out, payload, sizeof payload, &plen);
        spins++;
    }
    assert_int_equal(st, LIPC_OK);
    assert_int_equal((int)plen, 4);
    assert_memory_equal(payload, pl, 4);
    assert_int_equal(lipc_checksum_verify(out_wire, payload, plen), LIPC_OK);

    close(sp[0]);
    close(sp[1]);
}

static void test_frame_writer_socketpair(void **state)
{
    (void)state;
    int sp[2];

    assert_int_equal(socketpair(AF_UNIX, SOCK_STREAM, 0, sp), 0);

    lipc_header hdr = {
        .magic = LIPC_MAGIC,
        .version = LIPC_PROTOCOL_VERSION,
        .request = 3,
        .status = 0,
        .index = 1,
        .tid = 99,
        .length = 2,
        .checksum = 0,
        .value = 0xabcdull,
    };
    const char *pl = "xy";

    lipc_frame_writer wr;
    assert_int_equal(lipc_frame_writer_begin(&wr, &hdr, pl, 2), LIPC_OK);

    lipc_status st = LIPC_WOULD_BLOCK;
    int spins = 0;

    while (st == LIPC_WOULD_BLOCK && spins < 32) {
        st = lipc_frame_writer_write(sp[1], &wr);
        spins++;
    }
    assert_int_equal(st, LIPC_OK);

    uint8_t wire[LIPC_HEADER_SIZE];
    lipc_header rh;
    uint8_t payload[8];
    size_t plen = 0;

    assert_int_equal(lipc_frame_read(sp[0], LIPC_MAX_PAYLOAD_DEFAULT, wire, &rh, payload,
                          sizeof payload, &plen),
        LIPC_OK);
    assert_int_equal((int)plen, 2);
    assert_memory_equal(payload, pl, 2);
    assert_int_equal(rh.request, 3);
    assert_int_equal(rh.index, 1);
    assert_int_equal(rh.tid, 99u);
    assert_true(rh.value == 0xabcdull);

    close(sp[0]);
    close(sp[1]);
}

static void test_stream_split_header(void **state)
{
    (void)state;
    lipc_header in = {
        .magic = LIPC_PROTOCOL_MAGIC,
        .version = LIPC_PROTOCOL_VERSION,
        .request = 1,
        .status = 0,
        .index = 0,
        .tid = 1,
        .length = 3,
        .checksum = 0,
        .value = 0,
    };
    const char *pld = "abc";
    in.length = 3;
    uint8_t wire[LIPC_HEADER_SIZE];
    lipc_frame_fill_checksum(wire, &in, pld, 3);

    lipc_stream_rx rx;
    lipc_stream_rx_init(&rx);
    lipc_header out;
    uint8_t payload[16];
    size_t payload_len = 0;
    size_t off = 0;
    int done = 0;
    lipc_status st = lipc_stream_rx_feed(&rx, LIPC_MAX_PAYLOAD_DEFAULT, wire, 10, &off, &out,
        payload, sizeof payload, &payload_len, &done);
    assert_int_equal(st, LIPC_INCOMPLETE);
    assert_int_equal(done, 0);
    assert_int_equal((int)off, 10);

    st = lipc_stream_rx_feed(&rx, LIPC_MAX_PAYLOAD_DEFAULT, wire, 32, &off, &out, payload,
        sizeof payload, &payload_len, &done);
    assert_int_equal(st, LIPC_INCOMPLETE);
    assert_int_equal(done, 0);
    assert_int_equal((int)off, 32);

    off = 0;
    st = lipc_stream_rx_feed(&rx, LIPC_MAX_PAYLOAD_DEFAULT, (const uint8_t *)pld, 3, &off, &out,
        payload, sizeof payload, &payload_len, &done);
    assert_int_equal(st, LIPC_OK);
    assert_int_equal(done, 1);
    assert_int_equal(payload_len, 3);
    assert_memory_equal(payload, pld, 3);
}

static void test_socket_helpers_sockaddr_validation(void **state)
{
    (void)state;
    struct sockaddr_un addr;
    socklen_t len = 0;
    char long_path[sizeof(addr.sun_path) + 8];

    memset(long_path, 'a', sizeof(long_path));
    long_path[sizeof(long_path) - 1] = '\0';

    assert_non_null(lipc_default_socket_path());
    assert_true(strlen(lipc_default_socket_path()) > 0);
    assert_int_equal(lipc_sockaddr_init("/tmp/lipc_packet_test.sock", &addr, &len), LIPC_OK);
    assert_int_equal(addr.sun_family, AF_UNIX);
    assert_true(len > 0);
    assert_int_equal(strcmp(addr.sun_path, "/tmp/lipc_packet_test.sock"), 0);

    assert_int_equal(lipc_sockaddr_init(NULL, &addr, &len), LIPC_INVALID_PARAMS);
    assert_int_equal(lipc_sockaddr_init("", &addr, &len), LIPC_INVALID_PARAMS);
    assert_int_equal(lipc_sockaddr_init(long_path, &addr, &len), LIPC_BAD_LENGTH);
}

static void test_socket_helpers_fd_flags(void **state)
{
    (void)state;
    int sp[2];

    assert_int_equal(socketpair(AF_UNIX, SOCK_STREAM, 0, sp), 0);
    assert_int_equal(lipc_socket_set_nonblocking(sp[0], 1), LIPC_OK);
    assert_true((fcntl(sp[0], F_GETFL, 0) & O_NONBLOCK) != 0);
    assert_int_equal(lipc_socket_set_nonblocking(sp[0], 0), LIPC_OK);
    assert_true((fcntl(sp[0], F_GETFL, 0) & O_NONBLOCK) == 0);

    assert_int_equal(lipc_socket_set_cloexec(sp[1], 1), LIPC_OK);
    assert_true((fcntl(sp[1], F_GETFD, 0) & FD_CLOEXEC) != 0);
    assert_int_equal(lipc_socket_set_cloexec(sp[1], 0), LIPC_OK);
    assert_true((fcntl(sp[1], F_GETFD, 0) & FD_CLOEXEC) == 0);

    close(sp[0]);
    close(sp[1]);
}

static void test_socket_helpers_listen_connect(void **state)
{
    (void)state;
    char path[108];
    int listen_fd = -1;
    int client_fd = -1;
    int server_fd = -1;

    assert_true(snprintf(path, sizeof(path), "/tmp/lipc_socket_helper_%ld_%ld.sock",
                    (long)getpid(), random())
        < (int)sizeof(path));

    listen_fd = lipc_socket_listen(path, 4, LIPC_SOCKET_CLOEXEC);
    assert_true(listen_fd >= 0);
    client_fd = lipc_socket_connect(path, LIPC_SOCKET_CLOEXEC);
    assert_true(client_fd >= 0);

    server_fd = accept(listen_fd, NULL, NULL);
    assert_true(server_fd >= 0);

    close(server_fd);
    close(client_fd);
    close(listen_fd);
    unlink(path);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_status_helpers),
        cmocka_unit_test(test_header_round_trip),
        cmocka_unit_test(test_crc32_empty_payload_zlib_compat),
        cmocka_unit_test(test_crc32_payload_zlib_compat),
        cmocka_unit_test(test_header_bad_version),
        cmocka_unit_test(test_header_length_cap),
        cmocka_unit_test(test_frame_reader_nonblocking),
        cmocka_unit_test(test_frame_writer_socketpair),
        cmocka_unit_test(test_stream_split_header),
        cmocka_unit_test(test_socket_helpers_sockaddr_validation),
        cmocka_unit_test(test_socket_helpers_fd_flags),
        cmocka_unit_test(test_socket_helpers_listen_connect),
    };
    cmocka_set_message_output(CM_OUTPUT_TAP);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
