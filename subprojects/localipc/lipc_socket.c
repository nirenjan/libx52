/*
 * UNIX socket helpers for localipc.
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

#include <localipc/lipc.h>

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const char *lipc_default_socket_path(void)
{
    return LIPC_DEFAULT_SOCKET_PATH;
}

lipc_status lipc_sockaddr_init(const char *path, struct sockaddr_un *addr, socklen_t *len)
{
    size_t path_len;

    if (!path || !addr) {
        errno = EINVAL;
        return LIPC_INVALID_PARAMS;
    }
    if (path[0] == '\0') {
        errno = EINVAL;
        return LIPC_INVALID_PARAMS;
    }

    path_len = strlen(path);
    if (path_len >= sizeof(addr->sun_path)) {
        errno = ENAMETOOLONG;
        return LIPC_BAD_LENGTH;
    }

    memset(addr, 0, sizeof(*addr));
    addr->sun_family = AF_UNIX;
    memcpy(addr->sun_path, path, path_len + 1);
    if (len) {
        *len = (socklen_t)(offsetof(struct sockaddr_un, sun_path) + path_len + 1u);
    }
    return LIPC_OK;
}

lipc_status lipc_socket_set_nonblocking(int fd, int enabled)
{
    int flags;

    if (fd < 0) {
        errno = EINVAL;
        return LIPC_INVALID_PARAMS;
    }
    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return LIPC_IO_ERROR;
    }

    if (enabled) {
        if ((flags & O_NONBLOCK) != 0) {
            return LIPC_OK;
        }
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
            return LIPC_IO_ERROR;
        }
    } else {
        if ((flags & O_NONBLOCK) == 0) {
            return LIPC_OK;
        }
        if (fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) < 0) {
            return LIPC_IO_ERROR;
        }
    }

    return LIPC_OK;
}

lipc_status lipc_socket_set_cloexec(int fd, int enabled)
{
    int flags;

    if (fd < 0) {
        errno = EINVAL;
        return LIPC_INVALID_PARAMS;
    }
    flags = fcntl(fd, F_GETFD, 0);
    if (flags < 0) {
        return LIPC_IO_ERROR;
    }

    if (enabled) {
        if ((flags & FD_CLOEXEC) != 0) {
            return LIPC_OK;
        }
        if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0) {
            return LIPC_IO_ERROR;
        }
    } else {
        if ((flags & FD_CLOEXEC) == 0) {
            return LIPC_OK;
        }
        if (fcntl(fd, F_SETFD, flags & ~FD_CLOEXEC) < 0) {
            return LIPC_IO_ERROR;
        }
    }

    return LIPC_OK;
}

static int lipc_socket_create_stream(unsigned int flags)
{
    int type = SOCK_STREAM;
    int fd;

#ifdef SOCK_NONBLOCK
    if ((flags & LIPC_SOCKET_NONBLOCK) != 0u) {
        type |= SOCK_NONBLOCK;
    }
#endif
#ifdef SOCK_CLOEXEC
    if ((flags & LIPC_SOCKET_CLOEXEC) != 0u) {
        type |= SOCK_CLOEXEC;
    }
#endif

    fd = socket(AF_UNIX, type, 0);
    if (fd < 0) {
        return -1;
    }

#ifndef SOCK_NONBLOCK
    if ((flags & LIPC_SOCKET_NONBLOCK) != 0u
        && lipc_socket_set_nonblocking(fd, 1) != LIPC_OK) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
#else
    if ((flags & LIPC_SOCKET_NONBLOCK) != 0u
        && lipc_socket_set_nonblocking(fd, 1) != LIPC_OK) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
#endif

#ifndef SOCK_CLOEXEC
    if ((flags & LIPC_SOCKET_CLOEXEC) != 0u
        && lipc_socket_set_cloexec(fd, 1) != LIPC_OK) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
#else
    if ((flags & LIPC_SOCKET_CLOEXEC) != 0u
        && lipc_socket_set_cloexec(fd, 1) != LIPC_OK) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
#endif

    return fd;
}

int lipc_socket_listen(const char *path, int backlog, unsigned int flags)
{
    struct sockaddr_un addr;
    socklen_t addr_len = 0;
    int fd;

    if (backlog <= 0) {
        errno = EINVAL;
        return -1;
    }
    if (lipc_sockaddr_init(path, &addr, &addr_len) != LIPC_OK) {
        return -1;
    }

    fd = lipc_socket_create_stream(flags);
    if (fd < 0) {
        return -1;
    }

    unlink(path);
    if (bind(fd, (const struct sockaddr *)&addr, addr_len) < 0) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
    if (listen(fd, backlog) < 0) {
        int saved = errno;
        close(fd);
        unlink(path);
        errno = saved;
        return -1;
    }

    return fd;
}

int lipc_socket_connect(const char *path, unsigned int flags)
{
    struct sockaddr_un addr;
    socklen_t addr_len = 0;
    int fd;

    if (lipc_sockaddr_init(path, &addr, &addr_len) != LIPC_OK) {
        return -1;
    }

    fd = lipc_socket_create_stream(flags);
    if (fd < 0) {
        return -1;
    }

    if (connect(fd, (const struct sockaddr *)&addr, addr_len) < 0) {
        if ((flags & LIPC_SOCKET_NONBLOCK) != 0u
            && (errno == EINPROGRESS || errno == EALREADY)) {
            return fd;
        }
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }

    return fd;
}
