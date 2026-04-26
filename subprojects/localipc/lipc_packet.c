/*
 * LIPC wire header, zlib CRC32 framing, stream-safe reads.
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <localipc/lipc.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>

uint32_t lipc_crc32_update(uint32_t crc, const void *data, size_t len);

static void lipc_put16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)v;
}

static void lipc_put32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static void lipc_put64(uint8_t *p, uint64_t v)
{
    p[0] = (uint8_t)(v >> 56);
    p[1] = (uint8_t)(v >> 48);
    p[2] = (uint8_t)(v >> 40);
    p[3] = (uint8_t)(v >> 32);
    p[4] = (uint8_t)(v >> 24);
    p[5] = (uint8_t)(v >> 16);
    p[6] = (uint8_t)(v >> 8);
    p[7] = (uint8_t)v;
}

static uint16_t lipc_get16(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static uint32_t lipc_get32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static uint64_t lipc_get64(const uint8_t *p)
{
    return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) | ((uint64_t)p[2] << 40)
        | ((uint64_t)p[3] << 32) | ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16)
        | ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

int lipc_status_is_ok(lipc_status s)
{
    return s == LIPC_OK;
}

const char *lipc_status_str(lipc_status s)
{
    switch (s) {
    case LIPC_OK:
        return "OK";
    case LIPC_BAD_HEADER:
        return "BAD_HEADER";
    case LIPC_BAD_MAGIC:
        return "BAD_MAGIC";
    case LIPC_BAD_VERSION:
        return "BAD_VERSION";
    case LIPC_BAD_CHECKSUM:
        return "BAD_CHECKSUM";
    case LIPC_BAD_LENGTH:
        return "BAD_LENGTH";
    case LIPC_UNKNOWN_REQUEST:
        return "UNKNOWN_REQUEST";
    case LIPC_INVALID_PARAMS:
        return "INVALID_PARAMS";
    case LIPC_NOT_FOUND:
        return "NOT_FOUND";
    case LIPC_INTERNAL_ERROR:
        return "INTERNAL_ERROR";
    case LIPC_WOULD_BLOCK:
        return "WOULD_BLOCK";
    case LIPC_IO_ERROR:
        return "IO_ERROR";
    case LIPC_INCOMPLETE:
        return "INCOMPLETE";
    default:
        if ((uint16_t)s >= LIPC_STATUS_APP_BASE) {
            return "APPLICATION_DEFINED";
        }
        return "UNKNOWN_LIPC_STATUS";
    }
}

void lipc_header_encode(uint8_t wire[LIPC_HEADER_SIZE], const lipc_header *h)
{
    if (!wire || !h) {
        return;
    }
    lipc_put32(wire + LIPC_OFF_MAGIC, LIPC_PROTOCOL_MAGIC);
    lipc_put16(wire + LIPC_OFF_VERSION, LIPC_PROTOCOL_VERSION);
    lipc_put16(wire + LIPC_OFF_REQUEST, h->request);
    lipc_put16(wire + LIPC_OFF_STATUS, h->status);
    lipc_put16(wire + LIPC_OFF_INDEX, h->index);
    lipc_put32(wire + LIPC_OFF_TID, h->tid);
    lipc_put32(wire + LIPC_OFF_LENGTH, h->length);
    lipc_put32(wire + LIPC_OFF_CHECKSUM, 0);
    lipc_put64(wire + LIPC_OFF_VALUE, h->value);
}

lipc_status lipc_header_decode(lipc_header *h, const uint8_t wire[LIPC_HEADER_SIZE],
    uint32_t max_payload)
{
    if (!h || !wire) {
        return LIPC_BAD_HEADER;
    }

    h->magic = lipc_get32(wire + LIPC_OFF_MAGIC);
    h->version = lipc_get16(wire + LIPC_OFF_VERSION);
    h->request = lipc_get16(wire + LIPC_OFF_REQUEST);
    h->status = lipc_get16(wire + LIPC_OFF_STATUS);
    h->index = lipc_get16(wire + LIPC_OFF_INDEX);
    h->tid = lipc_get32(wire + LIPC_OFF_TID);
    h->length = lipc_get32(wire + LIPC_OFF_LENGTH);
    h->checksum = lipc_get32(wire + LIPC_OFF_CHECKSUM);
    h->value = lipc_get64(wire + LIPC_OFF_VALUE);

    if (h->magic != LIPC_PROTOCOL_MAGIC) {
        return LIPC_BAD_MAGIC;
    }
    if (h->version != LIPC_PROTOCOL_VERSION) {
        return LIPC_BAD_VERSION;
    }
    if (h->length > max_payload) {
        return LIPC_BAD_LENGTH;
    }
    return LIPC_OK;
}

void lipc_header_set_checksum(uint8_t wire[LIPC_HEADER_SIZE], uint32_t crc)
{
    lipc_put32(wire + LIPC_OFF_CHECKSUM, crc);
}

uint32_t lipc_checksum_compute(const uint8_t wire[LIPC_HEADER_SIZE], const void *payload,
    size_t payload_len)
{
    uint32_t c = lipc_crc32_update(0, wire, LIPC_HEADER_SIZE);
    if (payload_len > 0 && payload) {
        c = lipc_crc32_update(c, payload, payload_len);
    }
    return c;
}

lipc_status lipc_checksum_verify(const uint8_t wire[LIPC_HEADER_SIZE], const void *payload,
    size_t payload_len)
{
    uint32_t got = lipc_get32(wire + LIPC_OFF_CHECKSUM);
    uint8_t tmp[LIPC_HEADER_SIZE];

    memcpy(tmp, wire, sizeof tmp);
    lipc_put32(tmp + LIPC_OFF_CHECKSUM, 0);
    uint32_t expect = lipc_checksum_compute(tmp, payload, payload_len);
    if (expect != got) {
        return LIPC_BAD_CHECKSUM;
    }
    return LIPC_OK;
}

void lipc_frame_fill_checksum(uint8_t wire[LIPC_HEADER_SIZE], const lipc_header *h,
    const void *payload, size_t payload_len)
{
    if (!wire || !h) {
        return;
    }
    lipc_header_encode(wire, h);
    uint32_t crc = lipc_checksum_compute(wire, payload, payload_len);
    lipc_header_set_checksum(wire, crc);
}

static lipc_status lipc_rx_feed_exhaust(lipc_stream_rx *rx, uint32_t max_payload,
    const uint8_t *data, size_t data_len, size_t *data_off,
    lipc_header *out_hdr, uint8_t *payload_buf, size_t payload_cap, size_t *payload_len,
    int *frame_done)
{
    while (*data_off < data_len) {
        lipc_status st = lipc_stream_rx_feed(rx, max_payload, data, data_len, data_off, out_hdr,
            payload_buf, payload_cap, payload_len, frame_done);
        if (st != LIPC_OK && st != LIPC_INCOMPLETE) {
            return st;
        }
        if (*frame_done) {
            return LIPC_OK;
        }
        if (st == LIPC_INCOMPLETE && *data_off == data_len) {
            return LIPC_INCOMPLETE;
        }
    }
    return LIPC_INCOMPLETE;
}

void lipc_frame_reader_init(lipc_frame_reader *r)
{
    if (!r) {
        return;
    }
    lipc_stream_rx_init(&r->stream);
    r->pending_len = 0;
}

lipc_status lipc_frame_reader_read(int fd, lipc_frame_reader *r, uint32_t max_payload,
    uint8_t *scratch, size_t scratch_len,
    uint8_t *wire, lipc_header *h,
    void *payload_buf, size_t payload_cap, size_t *payload_len)
{
    if (!r || !scratch || scratch_len == 0 || scratch_len > LIPC_FRAME_READER_PENDING_CAP || !h
        || !payload_len || (payload_cap > 0 && !payload_buf)) {
        return LIPC_BAD_HEADER;
    }

    for (;;) {
        while (r->pending_len > 0) {
            size_t off = 0;
            int done = 0;
            lipc_status st = lipc_rx_feed_exhaust(&r->stream, max_payload, r->pending,
                r->pending_len, &off, h, (uint8_t *)payload_buf, payload_cap, payload_len, &done);
            if (st != LIPC_OK && st != LIPC_INCOMPLETE) {
                return st;
            }
            if (st == LIPC_OK && done) {
                size_t left = r->pending_len - off;
                if (left > 0) {
                    memmove(r->pending, r->pending + off, left);
                }
                r->pending_len = left;
                if (wire) {
                    lipc_header_encode(wire, h);
                    lipc_header_set_checksum(wire, h->checksum);
                }
                return LIPC_OK;
            }
            r->pending_len = 0;
            break;
        }

        ssize_t n = read(fd, scratch, scratch_len);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return LIPC_WOULD_BLOCK;
            }
            return LIPC_IO_ERROR;
        }
        if (n == 0) {
            errno = ECONNRESET;
            return LIPC_IO_ERROR;
        }

        size_t off = 0;
        int done = 0;
        lipc_status st = lipc_rx_feed_exhaust(&r->stream, max_payload, scratch, (size_t)n, &off,
            h, (uint8_t *)payload_buf, payload_cap, payload_len, &done);
        if (st != LIPC_OK && st != LIPC_INCOMPLETE) {
            return st;
        }
        if (st == LIPC_OK && done) {
            size_t left = (size_t)n - off;
            if (left > LIPC_FRAME_READER_PENDING_CAP) {
                return LIPC_BAD_LENGTH;
            }
            if (left > 0) {
                memcpy(r->pending, scratch + off, left);
                r->pending_len = left;
            }
            if (wire) {
                lipc_header_encode(wire, h);
                lipc_header_set_checksum(wire, h->checksum);
            }
            return LIPC_OK;
        }
    }
}

lipc_status lipc_frame_writer_begin(lipc_frame_writer *w, const lipc_header *h,
    const void *payload, size_t payload_len)
{
    if (!w || !h) {
        return LIPC_BAD_HEADER;
    }
    if (h->length != payload_len) {
        return LIPC_BAD_LENGTH;
    }
    if (payload_len > 0 && !payload) {
        return LIPC_BAD_HEADER;
    }
    lipc_frame_fill_checksum(w->hdr, h, payload, payload_len);
    w->payload = (const uint8_t *)payload;
    w->payload_len = payload_len;
    w->sent = 0;
    w->total = LIPC_HEADER_SIZE + payload_len;
    return LIPC_OK;
}

lipc_status lipc_frame_writer_write(int fd, lipc_frame_writer *w)
{
    if (!w) {
        return LIPC_BAD_HEADER;
    }
    while (w->sent < w->total) {
        const uint8_t *base;
        size_t chunk;

        if (w->sent < LIPC_HEADER_SIZE) {
            base = w->hdr + w->sent;
            chunk = LIPC_HEADER_SIZE - w->sent;
        } else {
            size_t poff = w->sent - LIPC_HEADER_SIZE;

            base = w->payload + poff;
            chunk = w->payload_len - poff;
        }
        ssize_t n = write(fd, base, chunk);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return LIPC_WOULD_BLOCK;
            }
            return LIPC_IO_ERROR;
        }
        if (n == 0) {
            errno = EPIPE;
            return LIPC_IO_ERROR;
        }
        w->sent += (size_t)n;
    }
    return LIPC_OK;
}

enum { LIPC_READ_FULL_OK = 0, LIPC_READ_FULL_ERR = -1, LIPC_READ_FULL_AGAIN = -2 };

static int lipc_read_full(int fd, void *buf, size_t len)
{
    uint8_t *p = buf;
    size_t got = 0;

    while (got < len) {
        ssize_t n = read(fd, p + got, len - got);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return LIPC_READ_FULL_AGAIN;
            }
            return LIPC_READ_FULL_ERR;
        }
        if (n == 0) {
            errno = ECONNRESET;
            return LIPC_READ_FULL_ERR;
        }
        got += (size_t)n;
    }
    return LIPC_READ_FULL_OK;
}

static int lipc_write_full(int fd, const void *buf, size_t len)
{
    const uint8_t *p = buf;
    size_t put = 0;

    while (put < len) {
        ssize_t n = write(fd, p + put, len - put);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return LIPC_READ_FULL_AGAIN;
            }
            return LIPC_READ_FULL_ERR;
        }
        put += (size_t)n;
    }
    return LIPC_READ_FULL_OK;
}

lipc_status lipc_frame_read(int fd, uint32_t max_payload, uint8_t wire[LIPC_HEADER_SIZE],
    lipc_header *h, void *payload_buf, size_t payload_cap, size_t *payload_len)
{
    if (!wire || !h || !payload_len || (payload_cap > 0 && !payload_buf)) {
        return LIPC_BAD_HEADER;
    }

    int rr = lipc_read_full(fd, wire, LIPC_HEADER_SIZE);
    if (rr == LIPC_READ_FULL_AGAIN) {
        return LIPC_WOULD_BLOCK;
    }
    if (rr != LIPC_READ_FULL_OK) {
        return LIPC_IO_ERROR;
    }

    lipc_status st = lipc_header_decode(h, wire, max_payload);
    if (st != LIPC_OK) {
        return st;
    }
    if (h->length > payload_cap) {
        return LIPC_BAD_LENGTH;
    }

    if (h->length > 0) {
        rr = lipc_read_full(fd, payload_buf, h->length);
        if (rr == LIPC_READ_FULL_AGAIN) {
            return LIPC_WOULD_BLOCK;
        }
        if (rr != LIPC_READ_FULL_OK) {
            return LIPC_IO_ERROR;
        }
    }
    st = lipc_checksum_verify(wire, payload_buf, h->length);
    if (st != LIPC_OK) {
        return st;
    }
    if (payload_len) {
        *payload_len = h->length;
    }
    return LIPC_OK;
}

lipc_status lipc_frame_write(int fd, lipc_header *h, const void *payload, size_t payload_len)
{
    if (!h || h->length != payload_len) {
        return LIPC_BAD_LENGTH;
    }
    if (payload_len > 0 && !payload) {
        return LIPC_BAD_HEADER;
    }

    uint8_t wire[LIPC_HEADER_SIZE];
    lipc_frame_fill_checksum(wire, h, payload, payload_len);

    int wr = lipc_write_full(fd, wire, sizeof wire);
    if (wr == LIPC_READ_FULL_AGAIN) {
        return LIPC_WOULD_BLOCK;
    }
    if (wr != LIPC_READ_FULL_OK) {
        return LIPC_IO_ERROR;
    }
    if (payload_len > 0 && payload) {
        wr = lipc_write_full(fd, payload, payload_len);
        if (wr == LIPC_READ_FULL_AGAIN) {
            return LIPC_WOULD_BLOCK;
        }
        if (wr != LIPC_READ_FULL_OK) {
            return LIPC_IO_ERROR;
        }
    }
    return LIPC_OK;
}

void lipc_stream_rx_init(lipc_stream_rx *rx)
{
    memset(rx, 0, sizeof *rx);
}

lipc_status lipc_stream_rx_feed(lipc_stream_rx *rx, uint32_t max_payload, const uint8_t *data,
    size_t data_len, size_t *data_off, lipc_header *out_hdr, uint8_t *payload_buf,
    size_t payload_cap, size_t *payload_len, int *frame_done)
{
    if (!rx || !data_off || !out_hdr || !payload_len || !frame_done) {
        return LIPC_BAD_HEADER;
    }
    *frame_done = 0;

    if (*data_off > data_len) {
        return LIPC_BAD_HEADER;
    }

    while (*data_off < data_len) {
        if (!rx->in_payload) {
            size_t need = LIPC_HEADER_SIZE - rx->header_got;
            size_t avail = data_len - *data_off;
            size_t take = need < avail ? need : avail;
            memcpy(rx->header + rx->header_got, data + *data_off, take);
            rx->header_got += take;
            *data_off += take;
            if (rx->header_got < LIPC_HEADER_SIZE) {
                return LIPC_INCOMPLETE;
            }

            lipc_status st = lipc_header_decode(out_hdr, rx->header, max_payload);
            if (st != LIPC_OK) {
                return st;
            }
            rx->payload_len = out_hdr->length;
            rx->payload_got = 0;
            rx->in_payload = 1;
            if (rx->payload_len > payload_cap) {
                return LIPC_BAD_LENGTH;
            }
            if (rx->payload_len == 0) {
                st = lipc_checksum_verify(rx->header, NULL, 0);
                if (st != LIPC_OK) {
                    return st;
                }
                *payload_len = 0;
                *frame_done = 1;
                lipc_stream_rx_init(rx);
                return LIPC_OK;
            }
        }

        size_t need = rx->payload_len - rx->payload_got;
        size_t avail = data_len - *data_off;
        size_t take = need < avail ? need : avail;
        if (take == 0) {
            return LIPC_INCOMPLETE;
        }
        memcpy(payload_buf + rx->payload_got, data + *data_off, take);
        rx->payload_got += take;
        *data_off += take;
        if (rx->payload_got < rx->payload_len) {
            return LIPC_INCOMPLETE;
        }

        lipc_status st = lipc_checksum_verify(rx->header, payload_buf, rx->payload_len);
        if (st != LIPC_OK) {
            return st;
        }
        *payload_len = rx->payload_len;
        *frame_done = 1;
        lipc_stream_rx_init(rx);
        return LIPC_OK;
    }

    return LIPC_INCOMPLETE;
}
