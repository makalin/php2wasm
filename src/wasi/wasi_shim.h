/**
 * WASI Shim Header
 * Provides WASI-compatible interfaces for PHP runtime
 */

#ifndef WASI_SHIM_H
#define WASI_SHIM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// WASI error codes
typedef uint16_t wasi_errno_t;

#define WASI_ESUCCESS     0
#define WASI_E2BIG         1
#define WASI_EACCES        2
#define WASI_EADDRINUSE    3
#define WASI_EADDRNOTAVAIL 4
#define WASI_EAFNOSUPPORT  5
#define WASI_EAGAIN        6
#define WASI_EALREADY      7
#define WASI_EBADF         8
#define WASI_EBADMSG       9
#define WASI_EBUSY         10
#define WASI_ECANCELED     11
#define WASI_ECHILD        12
#define WASI_ECONNABORTED  13
#define WASI_ECONNREFUSED  14
#define WASI_ECONNRESET    15
#define WASI_EDEADLK       16
#define WASI_EDESTADDRREQ  17
#define WASI_EDOM          18
#define WASI_EDQUOT        19
#define WASI_EEXIST        20
#define WASI_EFAULT        21
#define WASI_EFBIG         22
#define WASI_EHOSTUNREACH  23
#define WASI_EIDRM         24
#define WASI_EILSEQ        25
#define WASI_EINPROGRESS   26
#define WASI_EINTR         27
#define WASI_EINVAL        28
#define WASI_EIO           29
#define WASI_EISCONN       30
#define WASI_EISDIR        31
#define WASI_ELOOP         32
#define WASI_EMFILE        33
#define WASI_EMLINK        34
#define WASI_EMSGSIZE      35
#define WASI_EMULTIHOP     36
#define WASI_ENAMETOOLONG  37
#define WASI_ENETDOWN      38
#define WASI_ENETRESET     39
#define WASI_ENETUNREACH   40
#define WASI_ENFILE        41
#define WASI_ENOBUFS       42
#define WASI_ENODEV        43
#define WASI_ENOENT        44
#define WASI_ENOEXEC       45
#define WASI_ENOLCK        46
#define WASI_ENOLINK       47
#define WASI_ENOMEM        48
#define WASI_ENOMSG        49
#define WASI_ENOPROTOOPT   50
#define WASI_ENOSPC        51
#define WASI_ENOSYS        52
#define WASI_ENOTCONN      53
#define WASI_ENOTDIR       54
#define WASI_ENOTEMPTY     55
#define WASI_ENOTRECOVERABLE 56
#define WASI_ENOTSOCK      57
#define WASI_ENOTSUP       58
#define WASI_ENOTTY        59
#define WASI_ENXIO         60
#define WASI_EOVERFLOW     61
#define WASI_EOWNERDEAD    62
#define WASI_EPERM         63
#define WASI_EPIPE         64
#define WASI_EPROTO        65
#define WASI_EPROTONOSUPPORT 66
#define WASI_EPROTOTYPE    67
#define WASI_ERANGE        68
#define WASI_EROFS         69
#define WASI_ESPIPE        70
#define WASI_ESRCH         71
#define WASI_ESTALE        72
#define WASI_ETIMEDOUT     73
#define WASI_ETXTBSY       74
#define WASI_EXDEV         75
#define WASI_ENOTCAPABLE   76

// WASI file descriptor flags
typedef uint16_t wasi_fdflags_t;
#define WASI_FDFLAG_APPEND    0x0001
#define WASI_FDFLAG_DSYNC     0x0002
#define WASI_FDFLAG_NONBLOCK  0x0004
#define WASI_FDFLAG_RSYNC     0x0008
#define WASI_FDFLAG_SYNC      0x0010

// WASI file descriptor rights
typedef uint64_t wasi_rights_t;
#define WASI_RIGHT_FD_DATASYNC           0x0000000000000001
#define WASI_RIGHT_FD_READ               0x0000000000000002
#define WASI_RIGHT_FD_SEEK               0x0000000000000004
#define WASI_RIGHT_FD_FDSTAT_SET_FLAGS   0x0000000000000008
#define WASI_RIGHT_FD_SYNC               0x0000000000000010
#define WASI_RIGHT_FD_TELL               0x0000000000000020
#define WASI_RIGHT_FD_WRITE              0x0000000000000040
#define WASI_RIGHT_FD_ADVISE             0x0000000000000080
#define WASI_RIGHT_FD_ALLOCATE           0x0000000000000100
#define WASI_RIGHT_FD_READDIR            0x0000000000000200
#define WASI_RIGHT_PATH_CREATE_DIRECTORY 0x0000000000000400
#define WASI_RIGHT_PATH_CREATE_FILE      0x0000000000000800
#define WASI_RIGHT_PATH_LINK_SOURCE      0x0000000000001000
#define WASI_RIGHT_PATH_LINK_TARGET      0x0000000000002000
#define WASI_RIGHT_PATH_OPEN             0x0000000000004000
#define WASI_RIGHT_FD_READLINK           0x0000000000008000
#define WASI_RIGHT_PATH_RENAME_SOURCE    0x0000000000010000
#define WASI_RIGHT_PATH_RENAME_TARGET    0x0000000000020000
#define WASI_RIGHT_PATH_FILESTAT_GET     0x0000000000040000
#define WASI_RIGHT_PATH_FILESTAT_SET_SIZE 0x0000000000080000
#define WASI_RIGHT_PATH_FILESTAT_SET_TIMES 0x0000000000100000
#define WASI_RIGHT_FD_FILESTAT_GET       0x0000000000200000
#define WASI_RIGHT_FD_FILESTAT_SET_SIZE  0x0000000000400000
#define WASI_RIGHT_FD_FILESTAT_SET_TIMES 0x0000000000800000
#define WASI_RIGHT_PATH_SYMLINK          0x0000000001000000
#define WASI_RIGHT_PATH_REMOVE_DIRECTORY 0x0000000002000000
#define WASI_RIGHT_PATH_UNLINK_FILE      0x0000000004000000
#define WASI_RIGHT_POLL_FD_READWRITE     0x0000000008000000
#define WASI_RIGHT_SOCK_SHUTDOWN         0x0000000010000000

// WASI file descriptor
typedef uint32_t wasi_fd_t;

// WASI clock ID
typedef uint32_t wasi_clockid_t;
#define WASI_CLOCK_REALTIME           0
#define WASI_CLOCK_MONOTONIC          1
#define WASI_CLOCK_PROCESS_CPUTIME_ID 2
#define WASI_CLOCK_THREAD_CPUTIME_ID  3

// WASI timestamp
typedef uint64_t wasi_timestamp_t;

// WASI file type
typedef uint8_t wasi_filetype_t;
#define WASI_FILETYPE_UNKNOWN          0
#define WASI_FILETYPE_BLOCK_DEVICE     1
#define WASI_FILETYPE_CHARACTER_DEVICE 2
#define WASI_FILETYPE_DIRECTORY        3
#define WASI_FILETYPE_REGULAR_FILE     4
#define WASI_FILETYPE_SOCKET_DGRAM     5
#define WASI_FILETYPE_SOCKET_STREAM    6
#define WASI_FILETYPE_SYMBOLIC_LINK    7

// WASI file descriptor statistics
typedef struct {
    uint8_t filetype;
    uint16_t flags;
    uint8_t rights_base;
    uint8_t rights_inheriting;
} wasi_fdstat_t;

// WASI file statistics
typedef struct {
    uint8_t filetype;
    uint64_t nlink;
    uint64_t size;
    uint64_t atim;
    uint64_t mtim;
    uint64_t ctim;
} wasi_filestat_t;

// WASI directory entry
typedef struct {
    uint64_t d_next;
    uint64_t d_ino;
    uint64_t d_namlen;
    uint8_t d_type;
} wasi_dirent_t;

// WASI I/O vector
typedef struct {
    const uint8_t* buf;
    size_t len;
} wasi_iovec_t;

// WASI I/O vector for read
typedef struct {
    uint8_t* buf;
    size_t len;
} wasi_ciovec_t;

// Function declarations
bool wasi_init(void);
void wasi_cleanup(void);

// File operations
wasi_errno_t wasi_fd_read(wasi_fd_t fd, const wasi_iovec_t* iovs, size_t iovs_len, size_t* nread);
wasi_errno_t wasi_fd_write(wasi_fd_t fd, const wasi_ciovec_t* iovs, size_t iovs_len, size_t* nwritten);
wasi_errno_t wasi_fd_seek(wasi_fd_t fd, int64_t offset, uint8_t whence, uint64_t* newoffset);
wasi_errno_t wasi_fd_tell(wasi_fd_t fd, uint64_t* newoffset);
wasi_errno_t wasi_fd_close(wasi_fd_t fd);
wasi_errno_t wasi_fd_fdstat_get(wasi_fd_t fd, wasi_fdstat_t* stat);
wasi_errno_t wasi_fd_filestat_get(wasi_fd_t fd, wasi_filestat_t* stat);

// Directory operations
wasi_errno_t wasi_fd_readdir(wasi_fd_t fd, uint8_t* buf, size_t buf_len, uint64_t cookie, size_t* nread);
wasi_errno_t wasi_path_open(wasi_fd_t dirfd, uint32_t dirflags, const char* path, size_t path_len, 
                           wasi_rights_t rights_base, wasi_rights_t rights_inheriting, 
                           uint16_t fdflags, wasi_fd_t* fd);

// Clock operations
wasi_errno_t wasi_clock_time_get(wasi_clockid_t clock_id, wasi_timestamp_t precision, wasi_timestamp_t* time);

// Environment operations
wasi_errno_t wasi_environ_sizes_get(size_t* environ_count, size_t* environ_size);
wasi_errno_t wasi_environ_get(char** environ, size_t environ_buf_size);

// Arguments operations
wasi_errno_t wasi_args_sizes_get(size_t* argc, size_t* argv_buf_size);
wasi_errno_t wasi_args_get(char** argv, char* argv_buf);

// Random operations
wasi_errno_t wasi_random_get(uint8_t* buf, size_t buf_len);

// Process operations
void wasi_proc_exit(uint32_t exit_code);

#ifdef __cplusplus
}
#endif

#endif // WASI_SHIM_H
