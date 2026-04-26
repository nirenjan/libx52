#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <localipc/lipc.h>

enum {
    REQ_PING = 1,
    REQ_NOTIFY = 2,
};

struct completion_state {
    int done;
};

static lipc_status on_ping_reply(void *user, const lipc_header *hdr,
    const uint8_t *payload, size_t payload_len)
{
    struct completion_state *state = (struct completion_state *)user;

    printf("reply tid=%u status=%u request=%u payload_len=%zu\n",
        (unsigned)hdr->tid, (unsigned)hdr->status, (unsigned)hdr->request, payload_len);
    if (payload_len > 0 && payload != NULL) {
        printf("reply payload: %.*s\n", (int)payload_len, (const char *)payload);
    }

    state->done = 1;
    return LIPC_OK;
}

static lipc_status on_notify(void *user, const lipc_header *hdr,
    const uint8_t *payload, size_t payload_len)
{
    (void)user;
    printf("notify request=%u payload_len=%zu\n", (unsigned)hdr->request, payload_len);
    if (payload_len > 0 && payload != NULL) {
        printf("notify payload: %.*s\n", (int)payload_len, (const char *)payload);
    }
    return LIPC_OK;
}

int main(void)
{
    const char *socket_path = lipc_default_socket_path();
    static const uint8_t ping_payload[] = "ping";
    static const lipc_method_desc reply_desc = {
        .index = LIPC_FIELD_OPTIONAL,
        .value = LIPC_FIELD_OPTIONAL,
        .payload = LIPC_FIELD_OPTIONAL,
        .payload_min_len = 0,
        .payload_max_len = 1024,
    };
    static const lipc_method_desc notify_desc = {
        .index = LIPC_FIELD_FORBIDDEN,
        .value = LIPC_FIELD_FORBIDDEN,
        .payload = LIPC_FIELD_REQUIRED,
        .payload_min_len = 1,
        .payload_max_len = 1024,
    };
    static const lipc_client_reply_handler reply_handlers[] = {
        {
            .request_id = REQ_PING,
            .reply_desc = &reply_desc,
            .on_reply = on_ping_reply,
        },
    };
    static const lipc_client_notify_handler notify_handlers[] = {
        {
            .request_id = REQ_NOTIFY,
            .notify_desc = &notify_desc,
            .on_notify = on_notify,
        },
    };
    static const lipc_client_route_table routes = {
        .reply_handlers = reply_handlers,
        .n_reply = sizeof(reply_handlers) / sizeof(reply_handlers[0]),
        .notify_handlers = notify_handlers,
        .n_notify = sizeof(notify_handlers) / sizeof(notify_handlers[0]),
    };

    struct completion_state completion;
    lipc_client *client;
    lipc_status st;
    uint32_t tid = 0;
    int fd;

    completion.done = 0;
    client = lipc_client_create(LIPC_MAX_PAYLOAD_DEFAULT, &routes, &completion);
    if (client == NULL) {
        perror("lipc_client_create");
        return 1;
    } 

    fd = lipc_socket_connect(socket_path, 0u);
    if (fd < 0) {
        perror("lipc_socket_connect");
        lipc_client_destroy(client);
        return 1;
    }

    st = lipc_client_request_async(client, fd,
        REQ_PING,
        7,
        42,
        ping_payload,
        sizeof(ping_payload) - 1u,
        NULL,
        NULL,
        &tid);
    if (st != LIPC_OK) {
        fprintf(stderr, "request_async failed: %s\n", lipc_status_str(st));
        close(fd);
        lipc_client_destroy(client);
        return 1;
    }
    printf("request sent with tid=%u\n", tid);

    while (!completion.done) {
        st = lipc_client_poll_once(client, fd);
        if (st == LIPC_WOULD_BLOCK) {
            usleep(10 * 1000);
            continue;
        }

        if (st != LIPC_OK) {
            fprintf(stderr, "poll_once failed: %s\n", lipc_status_str(st));
            close(fd);
            lipc_client_destroy(client);
            return 1;
        }
    }

    close(fd);
    lipc_client_destroy(client);
    return 0;
}
