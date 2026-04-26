#ifndef LOCALIPC_LIPC_H
#define LOCALIPC_LIPC_H

/**
 * @file lipc.h
 * @brief Public API for liblocalipc framed UNIX-stream IPC.
 *
 * The library provides a protocol-generic request/response + notification transport with a
 * fixed big-endian wire header, zlib-compatible CRC32 checksums, framing helpers, and
 * table-driven dispatch for client/server endpoints.
 *
 * On-wire fixed header (32 bytes) matches this logical layout; @c payload bytes follow on the
 * stream and are not part of the header blob. The flexible array member appears only in the
 * documented shape below; @ref lipc_header is the fixed prefix only so it can be stack-allocated.
 *
 * @code
 * struct lipc_wire_layout {
 *     uint32_t magic;      // 0x4C495043 ("LIPC" as big-endian uint32_t)
 *     uint16_t version;    // 1
 *     uint16_t request;    // Request Type
 *     uint16_t status;     // Daemon Return Code
 *     uint16_t index;      // Optional Index
 *     uint32_t tid;        // Transaction ID (0 for Async)
 *     uint32_t length;     // Payload Length
 *     uint32_t checksum;   // CRC32
 *     uint64_t value;      // Optional inline 64-bit value
 *     uint8_t  payload[];  // Flexible array member (variable length; follows on the wire)
 * };
 * @endcode
 *
 * @example examples/lipc_server_example.c
 * @example examples/lipc_client_example.c
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>

/**
 * @defgroup lipc_packet Packet Format and Framing
 * Wire header fields, encoding/decoding, checksums, and stream-safe frame I/O.
 * @{
 */

/** liblocalipc API major version. */
#define LIPC_VERSION_MAJOR 0
/** liblocalipc API minor version. */
#define LIPC_VERSION_MINOR 1
/** liblocalipc API patch version. */
#define LIPC_VERSION_PATCH 0

/** Size of the fixed on-wire header (flexible array member is not included). */
#define LIPC_HEADER_SIZE 32u

/** Protocol magic: ASCII @c LIPC as a big-endian @c uint32_t (0x4C495043). */
#define LIPC_PROTOCOL_MAGIC 0x4C495043u

/** Alias for @ref LIPC_PROTOCOL_MAGIC (plan / shorthand). */
#define LIPC_MAGIC LIPC_PROTOCOL_MAGIC

/** Supported wire protocol version (header @c version field). */
#define LIPC_PROTOCOL_VERSION 1u

/**
 * Default maximum payload size (bytes) for @ref lipc_header_decode and framing helpers.
 * Callers may pass a lower cap per connection.
 */
#define LIPC_MAX_PAYLOAD_DEFAULT 65536u

/** Byte offsets of header fields (big-endian on the wire). */
#define LIPC_OFF_MAGIC 0u
#define LIPC_OFF_VERSION 4u
#define LIPC_OFF_REQUEST 6u
#define LIPC_OFF_STATUS 8u
#define LIPC_OFF_INDEX 10u
#define LIPC_OFF_TID 12u
#define LIPC_OFF_LENGTH 16u
#define LIPC_OFF_CHECKSUM 20u
#define LIPC_OFF_VALUE 24u

/** @} */

/**
 * @defgroup lipc_socket_helpers UNIX Socket Helpers
 * UNIX domain socket utilities used by both applications and liblocalipc internals.
 * @{
 */

/** Default filesystem socket path used by examples/tests. */
#define LIPC_DEFAULT_SOCKET_PATH "/tmp/lipc.sock"

/** Socket helper flag: configure fd/socket as non-blocking. */
#define LIPC_SOCKET_NONBLOCK (1u << 0)

/** Socket helper flag: configure fd/socket with close-on-exec. */
#define LIPC_SOCKET_CLOEXEC (1u << 1)

/** @} */

/**
 * @defgroup lipc_status_api Status and Error Handling
 * Result codes and helper utilities for protocol and runtime errors.
 * @{
 */

/**
 * First @c uint16_t value reserved for application-defined status codes on the wire.
 * Values in the range @c [LIPC_STATUS_APP_BASE, 0xFFFF] are not assigned by this library;
 * meaning (success, error, or sub-codes) is defined by the application protocol.
 * @ref lipc_status_is_ok is true only for @ref LIPC_OK (0); application codes are never
 * treated as success by that helper unless the application maps success to 0.
 */
#define LIPC_STATUS_APP_BASE 0x8000u

#if defined(_WIN32)
# define LIPC_API __declspec(dllexport)
#elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 303
# define LIPC_API __attribute__((visibility("default")))
#else
# define LIPC_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Wire status / result code: a @c uint16_t field in the header (big-endian on the wire).
 *
 * Library-defined values are @c 0 … @c 12. @ref LIPC_INCOMPLETE is parser-only (need more
 * bytes); it must not be sent as a packet status on the wire.
 *
 * Codes @c >= @ref LIPC_STATUS_APP_BASE (@c 0x8000) are reserved for application-specific use
 * (see macro documentation). They are not enumerated here so applications can define their
 * own constants without colliding with future library extensions below @c 0x8000.
 */
typedef enum lipc_status {
    LIPC_OK = 0,
    LIPC_BAD_HEADER = 1,
    LIPC_BAD_MAGIC = 2,
    LIPC_BAD_VERSION = 3,
    LIPC_BAD_CHECKSUM = 4,
    LIPC_BAD_LENGTH = 5,
    LIPC_UNKNOWN_REQUEST = 6,
    LIPC_INVALID_PARAMS = 7,
    LIPC_NOT_FOUND = 8,
    LIPC_INTERNAL_ERROR = 9,
    LIPC_WOULD_BLOCK = 10,
    LIPC_IO_ERROR = 11,
    /** Parser: need more bytes before a full frame is available. */
    LIPC_INCOMPLETE = 12,
} lipc_status;

/** @return Non-zero only if @p s is @ref LIPC_OK (application-defined codes are never “ok” here). */
LIPC_API int lipc_status_is_ok(lipc_status s);

/**
 * Short debug label for @p s (never NULL).
 * Library codes return an upper-snake token; @c s @c >= @ref LIPC_STATUS_APP_BASE yields
 * @c "APPLICATION_DEFINED"; other values yield @c "UNKNOWN_LIPC_STATUS".
 */
LIPC_API const char *lipc_status_str(lipc_status s);

/** @} */

/** @addtogroup lipc_packet
 * @{
 */

/** Fixed header fields in host byte order (payload is separate; see file comment). */
typedef struct lipc_header {
    uint32_t magic;    /**< Decoded wire magic; always @ref LIPC_PROTOCOL_MAGIC on valid packets. */
    uint16_t version;  /**< Decoded wire version; always @ref LIPC_PROTOCOL_VERSION on valid packets. */
    uint16_t request;  /**< Request/notification identifier used for table dispatch. */
    uint16_t status;   /**< Wire status code (library or application defined). */
    uint16_t index;    /**< Optional 16-bit scalar argument (method-specific semantics). */
    uint32_t tid;      /**< Transaction id; 0 means notification/push, non-zero means RPC. */
    uint32_t length;   /**< Payload length in bytes. */
    uint32_t checksum; /**< CRC32 from the wire image. */
    uint64_t value;    /**< Optional 64-bit scalar argument (method-specific semantics). */
} lipc_header;

/**
 * Encode @p h into exactly @ref LIPC_HEADER_SIZE bytes.
 * Writes @ref LIPC_PROTOCOL_MAGIC and @ref LIPC_PROTOCOL_VERSION ( @p h->magic / @p h->version
 * are not used for those wire fields). Sets the checksum field in @p wire to zero; use
 * @ref lipc_checksum_compute then @ref lipc_header_set_checksum before sending.
 */
LIPC_API void lipc_header_encode(uint8_t wire[LIPC_HEADER_SIZE], const lipc_header *h);

/**
 * Decode wire header into @p h. Validates magic, version (@ref LIPC_PROTOCOL_VERSION),
 * and @c length <= @p max_payload.
 */
LIPC_API lipc_status lipc_header_decode(lipc_header *h, const uint8_t wire[LIPC_HEADER_SIZE],
    uint32_t max_payload);

/** Write big-endian checksum into @p wire at @ref LIPC_OFF_CHECKSUM. */
LIPC_API void lipc_header_set_checksum(uint8_t wire[LIPC_HEADER_SIZE], uint32_t crc);

/**
 * Zlib-compatible CRC-32 (reflected, poly 0xEDB88320), same as Python @c zlib.crc32.
 * Covers @p wire (checksum field must already be zero) followed by @p payload.
 */
LIPC_API uint32_t lipc_checksum_compute(const uint8_t wire[LIPC_HEADER_SIZE],
    const void *payload, size_t payload_len);

/**
 * Verify checksum for a received header + payload.
 * @return @ref LIPC_OK or @ref LIPC_BAD_CHECKSUM.
 */
LIPC_API lipc_status lipc_checksum_verify(const uint8_t wire[LIPC_HEADER_SIZE],
    const void *payload, size_t payload_len);

/**
 * Prepare an outgoing frame: encode header with checksum 0, compute CRC, patch checksum.
 * @p payload may be NULL if @p payload_len is 0.
 */
LIPC_API void lipc_frame_fill_checksum(uint8_t wire[LIPC_HEADER_SIZE], const lipc_header *h,
    const void *payload, size_t payload_len);

/**
 * Read one complete frame from @p fd (blocking partial reads until one full frame).
 * Suitable when @p fd is blocking: a non-blocking socket can return @ref LIPC_WOULD_BLOCK after
 * a partial header/payload and lose sync — use @ref lipc_frame_reader_read instead.
 * On success, @p wire contains the header and @p payload_buf holds @p *payload_len bytes.
 * @return @ref LIPC_OK, @ref LIPC_IO_ERROR (check @c errno), @ref LIPC_WOULD_BLOCK for
 *         @c EAGAIN / @c EWOULDBLOCK, or another validation status.
 */
LIPC_API lipc_status lipc_frame_read(int fd, uint32_t max_payload,
    uint8_t wire[LIPC_HEADER_SIZE], lipc_header *h,
    void *payload_buf, size_t payload_cap, size_t *payload_len);

/**
 * Write one complete frame to @p fd (header + payload). Fills checksum via @ref lipc_frame_fill_checksum.
 * Uses a blocking write loop; for non-blocking @p fd use @ref lipc_frame_writer_write.
 * @return @ref LIPC_OK, @ref LIPC_IO_ERROR (check @c errno), @ref LIPC_WOULD_BLOCK for
 *         @c EAGAIN / @c EWOULDBLOCK, or @ref LIPC_BAD_LENGTH if @c h->length does not match @p payload_len.
 */
LIPC_API lipc_status lipc_frame_write(int fd, lipc_header *h,
    const void *payload, size_t payload_len);

/** Streaming receive state (one frame at a time). */
typedef struct lipc_stream_rx {
    uint8_t header[LIPC_HEADER_SIZE]; /**< Partially collected header bytes. */
    size_t header_got;                /**< Number of bytes currently in @ref header. */
    uint32_t payload_len;             /**< Payload length of the frame currently being assembled. */
    size_t payload_got;               /**< Number of payload bytes collected so far. */
    int in_payload; /**< 0 = filling header, non-zero = filling payload */
} lipc_stream_rx;

/** Reset @p rx to its initial state before first use or after error recovery. */
LIPC_API void lipc_stream_rx_init(lipc_stream_rx *rx);

/**
 * Feed incoming stream bytes. Consumes from @p data starting at @p *data_off (updated on success).
 * When @p *frame_done is non-zero, @p out_hdr and @p payload_buf (first @p *payload_len bytes) are valid.
 * @return @ref LIPC_OK when a frame is complete, @ref LIPC_INCOMPLETE when more data is needed,
 *         or another error status.
 */
LIPC_API lipc_status lipc_stream_rx_feed(lipc_stream_rx *rx, uint32_t max_payload,
    const uint8_t *data, size_t data_len, size_t *data_off,
    lipc_header *out_hdr, uint8_t *payload_buf, size_t payload_cap, size_t *payload_len,
    int *frame_done);

/** Internal buffer size for @ref lipc_frame_reader (must fit a stashed tail after @p scratch). */
#define LIPC_FRAME_READER_PENDING_CAP 8192u

/** Stream-safe framed read state (non-blocking or mixed). */
typedef struct lipc_frame_reader {
    lipc_stream_rx stream;                            /**< Incremental decode state machine. */
    uint8_t pending[LIPC_FRAME_READER_PENDING_CAP];  /**< Buffered bytes left after a frame boundary. */
    size_t pending_len;                               /**< Number of valid bytes in @ref pending. */
} lipc_frame_reader;

/** Initialize @p r for use with @ref lipc_frame_reader_read. */
LIPC_API void lipc_frame_reader_init(lipc_frame_reader *r);

/**
 * Read one full frame, preserving partial progress across @c EAGAIN.
 * @p scratch_len must be <= @ref LIPC_FRAME_READER_PENDING_CAP.
 * If @p wire is non-NULL, fills it with the validated on-wire header (including checksum).
 */
LIPC_API lipc_status lipc_frame_reader_read(int fd, lipc_frame_reader *r, uint32_t max_payload,
    uint8_t *scratch, size_t scratch_len,
    uint8_t *wire, lipc_header *h,
    void *payload_buf, size_t payload_cap, size_t *payload_len);

/** Incremental framed write (non-blocking safe). */
typedef struct lipc_frame_writer {
    uint8_t hdr[LIPC_HEADER_SIZE]; /**< Encoded header with finalized checksum. */
    const uint8_t *payload;        /**< Borrowed payload pointer provided to @ref lipc_frame_writer_begin. */
    size_t payload_len;            /**< Payload size in bytes. */
    size_t sent;                   /**< Bytes already written across header+payload. */
    size_t total;                  /**< Total bytes to write (header + payload). */
} lipc_frame_writer;

/** Validates @c h->length == @p payload_len, fills @p w. Checksum is computed once here. */
LIPC_API lipc_status lipc_frame_writer_begin(lipc_frame_writer *w, const lipc_header *h,
    const void *payload, size_t payload_len);

/**
 * Send more bytes of the current frame. Call until it returns @ref LIPC_OK.
 * @return @ref LIPC_OK when finished, @ref LIPC_WOULD_BLOCK if the last @c write would block,
 *         or another status on failure (@c errno set when @ref LIPC_IO_ERROR).
 */
LIPC_API lipc_status lipc_frame_writer_write(int fd, lipc_frame_writer *w);

/** @} */

/**
 * @defgroup lipc_dispatch Method Validation and Dispatch Tables
 * Declarative method descriptors shared by server and client routing.
 * @{
 */

/* -------------------------------------------------------------------------- */
/* Method descriptors, server dispatch, client notify vs reply routing         */
/* -------------------------------------------------------------------------- */

/**
 * How @ref lipc_method_desc treats @c index, @c value, and payload length on the wire.
 *
 * - @ref LIPC_FIELD_FORBIDDEN — @c index / @c value must be 0; payload length must be 0.
 * - @ref LIPC_FIELD_OPTIONAL — any value allowed; payload must still satisfy
 *   @c payload_min_len / @c payload_max_len (use 0 and @c UINT32_MAX-style caps as needed).
 * - @ref LIPC_FIELD_REQUIRED — @c index / @c value must be non-zero; payload length must be within
 *   @c payload_min_len … @c payload_max_len inclusive (set bounds for “empty but required” as min=0
 *   if that is valid for your method).
 */
typedef enum lipc_field_rule {
    LIPC_FIELD_FORBIDDEN = 0,
    LIPC_FIELD_OPTIONAL = 1,
    LIPC_FIELD_REQUIRED = 2,
} lipc_field_rule;

/**
 * Per-method validation for incoming headers + payload length.
 * For @c payload, @c payload_max_len is clamped by the caller’s frame cap (e.g. @ref LIPC_MAX_PAYLOAD_DEFAULT).
 */
typedef struct lipc_method_desc {
    lipc_field_rule index;      /**< Rule applied to @ref lipc_header.index. */
    lipc_field_rule value;      /**< Rule applied to @ref lipc_header.value. */
    lipc_field_rule payload;    /**< Rule applied to payload length/presence. */
    uint32_t payload_min_len;   /**< Inclusive minimum payload length when payload is optional/required. */
    uint32_t payload_max_len;   /**< Inclusive maximum payload length when payload is optional/required. */
} lipc_method_desc;

/**
 * Validate @p h and @p payload_len against @p desc.
 * @return @ref LIPC_OK or @ref LIPC_INVALID_PARAMS.
 */
LIPC_API lipc_status lipc_method_validate(const lipc_method_desc *desc, const lipc_header *h,
    size_t payload_len);

/** Server reply context: set by the library before your handler runs; pass to @ref lipc_server_reply. */
typedef struct lipc_server_reply_ctx {
    int fd;                 /**< Connected client fd to which replies should be sent. */
    lipc_header request_hdr; /**< Original decoded request header associated with this callback. */
} lipc_server_reply_ctx;

/** Opaque server context owned by liblocalipc. */
typedef struct lipc_server lipc_server;

/**
 * Server callback invoked by @ref lipc_server_dispatch and the run loop.
 *
 * The callback executes on the server I/O thread (the thread running @ref lipc_server_run),
 * so it should avoid long blocking work. @p payload points to a library-owned receive buffer and
 * is only valid for the duration of the callback.
 */
typedef lipc_status (*lipc_server_fn)(void *user, lipc_server_reply_ctx *reply,
    const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len);

/** One server registration: @c request_id, validation for the incoming request, handler. */
typedef struct lipc_server_handler {
    uint16_t request_id;                  /**< Request id matched against @ref lipc_header.request. */
    const lipc_method_desc *request_desc; /**< Validation contract for request fields/payload. */
    lipc_server_fn fn;                    /**< Handler callback for matched requests. */
} lipc_server_handler;

/**
 * @defgroup lipc_server Server API
 * Server lifecycle, run loop control, and request dispatch helpers.
 * @{
 */

/**
 * Create a server context around an already-bound/listening UNIX stream socket @p listen_fd.
 *
 * Ownership/lifetime:
 * - Ownership of @p listen_fd transfers to the returned server on success.
 * - On success, @p handlers is copied internally; caller may free/reuse its original array.
 * - Returns NULL on allocation/setup failure; in that case @p listen_fd remains caller-owned.
 *
 * Threading:
 * - Safe before @ref lipc_server_run starts.
 * - Do not call concurrently with @ref lipc_server_destroy.
 *
 * Error reporting:
 * - Returns NULL on failure and may leave @c errno set by allocation or OS calls.
 */
LIPC_API lipc_server *lipc_server_create(int listen_fd, uint32_t max_payload,
    const lipc_server_handler *handlers, size_t nhandlers, void *handler_user);

/**
 * Destroy a server and release all owned resources.
 *
 * This closes all connected client fds, wake-pipe fds, and the owned listening fd.
 * Passing NULL is a no-op.
 *
 * Threading:
 * - Not safe concurrently with @ref lipc_server_run.
 * - Stop the run loop and join your run thread before destroy.
 */
LIPC_API void lipc_server_destroy(lipc_server *server);

/**
 * Wake a running @ref lipc_server_run poll loop from any thread.
 *
 * @return @ref LIPC_OK on success (including already-awake pipe), @ref LIPC_IO_ERROR on wake-fd
 *         write failure, or @ref LIPC_BAD_HEADER if @p server is invalid/uninitialized.
 * @note When @ref LIPC_IO_ERROR is returned, @c errno is set by @c write.
 */
LIPC_API lipc_status lipc_server_wake(lipc_server *server);

/**
 * Request shutdown and wake @ref lipc_server_run from any thread.
 *
 * The run loop exits with @ref LIPC_OK once it observes the stop request.
 * This function is idempotent.
 */
LIPC_API lipc_status lipc_server_stop(lipc_server *server);

/**
 * Blocking server loop: poll listener + connected clients + wake fd, perform framing/dispatch.
 * Returns @ref LIPC_OK after @ref lipc_server_stop, or an error status on fatal setup/runtime failure.
 *
 * Threading:
 * - Exactly one thread may execute @ref lipc_server_run per server instance.
 * - Handler callbacks execute on the @ref lipc_server_run thread.
 * - @ref lipc_server_wake and @ref lipc_server_stop are safe from other threads.
 *
 * Error/IO:
 * - Returns @ref LIPC_IO_ERROR for fatal @c poll/@c accept/read/write failures.
 * - For per-client protocol failures (bad checksum/header/etc.), the client connection is dropped
 *   and the loop continues serving other clients.
 */
LIPC_API lipc_status lipc_server_run(lipc_server *server);

/**
 * Send one response frame echoing request @c tid and @c request from @p reply->request_hdr.
 *
 * Ownership:
 * - @p payload is borrowed for the duration of the call only.
 *
 * Error mapping:
 * - Returns @ref LIPC_BAD_LENGTH when payload cannot be represented in @c uint32_t length.
 * - Returns @ref LIPC_IO_ERROR/@ref LIPC_WOULD_BLOCK from underlying framed write operations.
 * - On @ref LIPC_IO_ERROR, @c errno is set by underlying write calls.
 */
LIPC_API lipc_status lipc_server_reply(lipc_server_reply_ctx *reply, uint16_t wire_status,
    uint16_t index, uint64_t value, const void *payload, size_t payload_len);

/**
 * Find a handler for @p request_id in @p handlers (linear search; first match wins).
 * @return Pointer into @p handlers, or NULL when no match exists.
 */
LIPC_API const lipc_server_handler *lipc_server_handler_lookup(const lipc_server_handler *handlers,
    size_t nhandlers, uint16_t request_id);

/**
 * Handle one validated request: rejects client @c tid == 0 (@ref LIPC_INVALID_PARAMS), looks up handler,
 * validates fields with @c request_desc, then calls the handler. The handler should call @ref lipc_server_reply.
 * @return Status from validation / lookup (@ref LIPC_UNKNOWN_REQUEST, etc.) or the handler’s return value.
 */
LIPC_API lipc_status lipc_server_dispatch(const lipc_server_handler *handlers, size_t nhandlers,
    void *user, int fd, const lipc_header *req_hdr, const uint8_t *payload, size_t payload_len);

/** @} */

/**
 * @defgroup lipc_client Client API
 * Client lifecycle, routing tables, request submission, and synchronous/asynchronous receive paths.
 * @{
 */

/**
 * Client callback for reply-table dispatch when @c hdr->tid != 0.
 *
 * @param user User pointer from @ref lipc_client_create route table registration.
 * @param hdr Decoded header for the incoming reply.
 * @param payload Borrowed pointer to payload bytes (valid only for callback duration).
 * @param payload_len Number of bytes at @p payload.
 * @return @ref LIPC_OK to indicate successful handling, or a domain-specific status.
 */
typedef lipc_status (*lipc_client_reply_cb)(void *user, const lipc_header *hdr, const uint8_t *payload,
    size_t payload_len);

/**
 * Client callback for notification-table dispatch when @c hdr->tid == 0.
 *
 * The same lifetime and return-value rules as @ref lipc_client_reply_cb apply.
 */
typedef lipc_status (*lipc_client_notify_cb)(void *user, const lipc_header *hdr, const uint8_t *payload,
    size_t payload_len);

/** Opaque client context owned by liblocalipc. */
typedef struct lipc_client lipc_client;

/**
 * Per-request completion callback for @ref lipc_client_request_async.
 *
 * It is invoked exactly once for a matched reply or never if the request fails before send.
 * The callback executes on the thread calling @ref lipc_client_dispatch_frame,
 * @ref lipc_client_poll_once, or @ref lipc_client_call.
 */
typedef lipc_status (*lipc_client_completion_cb)(void *user, uint32_t tid,
    const lipc_header *hdr, const uint8_t *payload, size_t payload_len);

/** Client-side handler for RPC replies (@c tid != 0). */
typedef struct lipc_client_reply_handler {
    uint16_t request_id;                /**< Reply request id to match against @ref lipc_header.request. */
    const lipc_method_desc *reply_desc; /**< Validation contract for matched replies. */
    lipc_client_reply_cb on_reply;      /**< Reply callback invoked after validation. */
} lipc_client_reply_handler;

/** Client-side handler for server pushes (@c tid == 0). */
typedef struct lipc_client_notify_handler {
    uint16_t request_id;                  /**< Notification request id to match. */
    const lipc_method_desc *notify_desc;  /**< Validation contract for matched notifications. */
    lipc_client_notify_cb on_notify;      /**< Notification callback invoked after validation. */
} lipc_client_notify_handler;

/** Route table copied at @ref lipc_client_create time. */
typedef struct lipc_client_route_table {
    const lipc_client_reply_handler *reply_handlers;   /**< Reply handlers array (may be NULL). */
    size_t n_reply;                                    /**< Entries in @ref reply_handlers. */
    const lipc_client_notify_handler *notify_handlers; /**< Notification handlers array (may be NULL). */
    size_t n_notify;                                   /**< Entries in @ref notify_handlers. */
} lipc_client_route_table;

/** Find a reply route by request id; returns NULL when not present. */
LIPC_API const lipc_client_reply_handler *lipc_client_reply_lookup(
    const lipc_client_reply_handler *handlers, size_t nhandlers, uint16_t request_id);

/** Find a notify route by request id; returns NULL when not present. */
LIPC_API const lipc_client_notify_handler *lipc_client_notify_lookup(
    const lipc_client_notify_handler *handlers, size_t nhandlers, uint16_t request_id);

/**
 * Route one incoming client-side frame after decode + CRC.
 * - @c hdr->tid == 0: dispatch to @c notify_handlers by @c hdr->request; @ref LIPC_UNKNOWN_REQUEST if unlisted.
 * - @c hdr->tid != 0: dispatch to @c reply_handlers by @c hdr->request; @ref LIPC_UNKNOWN_REQUEST if unlisted.
 * Validates against the matching descriptor before invoking the callback.
 */
LIPC_API lipc_status lipc_client_dispatch_incoming(const lipc_client_route_table *routes, void *user,
    const lipc_header *hdr, const uint8_t *payload, size_t payload_len);

/**
 * Create a client context with an internal non-zero TID allocator and in-flight table.
 * @p max_payload of 0 uses @ref LIPC_MAX_PAYLOAD_DEFAULT.
 *
 * Ownership/lifetime:
 * - The route arrays referenced by @p routes are copied; caller retains ownership of originals.
 * - Returned client must be released with @ref lipc_client_destroy.
 *
 * Threading:
 * - Safe from any thread before the client is published to other threads.
 *
 * Error reporting:
 * - Returns NULL on allocation/setup failure.
 */
LIPC_API lipc_client *lipc_client_create(uint32_t max_payload,
    const lipc_client_route_table *routes, void *route_user);

/**
 * Free client resources and any in-flight entries.
 *
 * Threading:
 * - Not safe concurrently with other client APIs.
 * - Ensure no other thread is inside client APIs before destroy.
 *
 * Passing NULL is a no-op.
 */
LIPC_API void lipc_client_destroy(lipc_client *client);

/**
 * Allocate a non-zero transaction ID that is not currently in-flight.
 *
 * Threading:
 * - Safe concurrently with @ref lipc_client_request_async / dispatch APIs.
 *
 * @return @ref LIPC_OK on success, @ref LIPC_BAD_HEADER for invalid args, or
 *         @ref LIPC_INTERNAL_ERROR if no free TID can be allocated.
 */
LIPC_API lipc_status lipc_client_next_tid(lipc_client *client, uint32_t *out_tid);

/**
 * Send a client request with an internally allocated non-zero TID.
 * The completion callback is keyed by the allocated TID and invoked when the matching reply arrives.
 *
 * Threading:
 * - Safe to call concurrently from multiple threads.
 * - Completion callbacks run on the thread that calls @ref lipc_client_dispatch_frame /
 *   @ref lipc_client_poll_once / @ref lipc_client_call.
 *
 * Ownership/lifetime:
 * - @p payload is borrowed for the duration of this call only.
 * - On success, completion ownership transfers to the internal in-flight table until reply/cleanup.
 *
 * Error/IO:
 * - Returns @ref LIPC_BAD_LENGTH when @p payload_len cannot fit in wire length.
 * - Returns @ref LIPC_IO_ERROR/@ref LIPC_WOULD_BLOCK from socket write path.
 * - On @ref LIPC_IO_ERROR, @c errno is set by underlying write.
 */
LIPC_API lipc_status lipc_client_request_async(lipc_client *client, int fd,
    uint16_t request_id, uint16_t index, uint64_t value,
    const void *payload, size_t payload_len,
    lipc_client_completion_cb on_complete, void *complete_user,
    uint32_t *out_tid);

/**
 * Route one decoded frame through in-flight tracking:
 * - @c tid == 0: notification path via @ref lipc_client_dispatch_incoming.
 * - @c tid != 0: resolve by in-flight TID and invoke that request's completion callback.
 *
 * Threading:
 * - Safe concurrently with @ref lipc_client_request_async and @ref lipc_client_next_tid.
 *
 * @return @ref LIPC_NOT_FOUND when reply TID has no matching in-flight request.
 */
LIPC_API lipc_status lipc_client_dispatch_frame(lipc_client *client,
    const lipc_header *hdr, const uint8_t *payload, size_t payload_len);

/**
 * Read and dispatch exactly one frame from @p fd.
 * Returns @ref LIPC_WOULD_BLOCK for non-blocking fds with no full frame ready.
 *
 * Threading:
 * - Serialized per-client internally; concurrent callers are allowed but one frame is
 *   read/decoded at a time per client context.
 *
 * Error/IO:
 * - Returns protocol validation statuses for malformed frames.
 * - Returns @ref LIPC_IO_ERROR for read failures (with @c errno set by OS read/poll path).
 */
LIPC_API lipc_status lipc_client_poll_once(lipc_client *client, int fd);

/**
 * Synchronous helper: sends a request with an internal TID and blocks until its matching reply.
 * While waiting, unrelated notifications are still dispatched.
 *
 * @p reply_payload may be NULL only if @p reply_payload_cap is 0.
 * @p reply_hdr may be NULL when the caller only needs payload bytes.
 *
 * Blocking/threading:
 * - Blocks the calling thread until matching reply or error.
 * - Safe concurrently with other request-producing APIs on the same client.
 *
 * Error/IO:
 * - Returns @ref LIPC_BAD_LENGTH if a matched reply payload exceeds @p reply_payload_cap.
 * - Returns @ref LIPC_IO_ERROR for fatal socket polling/read failures (with @c errno set).
 */
LIPC_API lipc_status lipc_client_call(lipc_client *client, int fd,
    uint16_t request_id, uint16_t index, uint64_t value,
    const void *payload, size_t payload_len,
    lipc_header *reply_hdr, void *reply_payload, size_t reply_payload_cap, size_t *reply_len);

/** @} */
/** @} */

/** @addtogroup lipc_socket_helpers
 * @{
 */

/* -------------------------------------------------------------------------- */
/* UNIX socket helper utilities                                               */
/* -------------------------------------------------------------------------- */

/** Canonical default UNIX socket path used by localipc helpers/examples. */
LIPC_API const char *lipc_default_socket_path(void);

/**
 * Initialize a UNIX sockaddr from @p path and optionally return its length in @p len.
 * Does not create any file descriptor.
 *
 * @param path NUL-terminated filesystem path.
 * @param addr Output structure to fill.
 * @param len Optional output length for @c bind/@c connect.
 * @return @ref LIPC_OK, @ref LIPC_INVALID_PARAMS for bad input, or @ref LIPC_BAD_LENGTH.
 * @note On failure, @c errno is set to @c EINVAL or @c ENAMETOOLONG.
 */
LIPC_API lipc_status lipc_sockaddr_init(const char *path, struct sockaddr_un *addr, socklen_t *len);

/**
 * Set or clear non-blocking mode on @p fd.
 * @return @ref LIPC_OK, @ref LIPC_INVALID_PARAMS, or @ref LIPC_IO_ERROR (@c errno preserved).
 */
LIPC_API lipc_status lipc_socket_set_nonblocking(int fd, int enabled);

/**
 * Set or clear close-on-exec on @p fd.
 * @return @ref LIPC_OK, @ref LIPC_INVALID_PARAMS, or @ref LIPC_IO_ERROR (@c errno preserved).
 */
LIPC_API lipc_status lipc_socket_set_cloexec(int fd, int enabled);

/**
 * Create+bind+listen a UNIX stream socket at @p path.
 * @param flags Bitwise OR of @ref LIPC_SOCKET_NONBLOCK and @ref LIPC_SOCKET_CLOEXEC.
 * @param backlog Positive backlog passed to @c listen.
 * @return listening fd on success, -1 on error (@c errno set).
 *
 * Ownership:
 * - Caller owns the returned fd and must close it.
 * - Existing socket path is unlinked before bind.
 */
LIPC_API int lipc_socket_listen(const char *path, int backlog, unsigned int flags);

/**
 * Create+connect a UNIX stream socket to @p path.
 * @param flags Bitwise OR of @ref LIPC_SOCKET_NONBLOCK and @ref LIPC_SOCKET_CLOEXEC.
 * @return connected fd on success, -1 on error (@c errno set).
 *
 * Ownership:
 * - Caller owns the returned fd and must close it.
 * - In non-blocking mode, @c EINPROGRESS/@c EALREADY are treated as success with a
 *   still-connecting fd return value.
 */
LIPC_API int lipc_socket_connect(const char *path, unsigned int flags);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LOCALIPC_LIPC_H */
