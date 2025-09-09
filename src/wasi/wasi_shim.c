/**
 * WASI Shim Implementation
 * Provides WASI-compatible interfaces for PHP runtime
 */

#include "wasi_shim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

// Standard file descriptors
#define WASI_STDIN_FD  0
#define WASI_STDOUT_FD 1
#define WASI_STDERR_FD 2

// Global state
static bool wasi_initialized = false;
static char** wasi_argv = NULL;
static char** wasi_envp = NULL;
static int wasi_argc = 0;

bool wasi_init(void) {
    if (wasi_initialized) {
        return true;
    }

    // Initialize standard file descriptors
    // These are typically handled by the WASI runtime
    
    wasi_initialized = true;
    return true;
}

void wasi_cleanup(void) {
    if (!wasi_initialized) {
        return;
    }

    // Cleanup resources
    if (wasi_argv) {
        free(wasi_argv);
        wasi_argv = NULL;
    }
    
    if (wasi_envp) {
        free(wasi_envp);
        wasi_envp = NULL;
    }

    wasi_initialized = false;
}

// File operations
wasi_errno_t wasi_fd_read(wasi_fd_t fd, const wasi_iovec_t* iovs, size_t iovs_len, size_t* nread) {
    if (!iovs || !nread) {
        return WASI_EINVAL;
    }

    *nread = 0;
    
    for (size_t i = 0; i < iovs_len; i++) {
        ssize_t result = read(fd, (void*)iovs[i].buf, iovs[i].len);
        if (result < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return WASI_EAGAIN;
            }
            return WASI_EIO;
        }
        *nread += result;
        
        // If we read less than requested, we're done
        if ((size_t)result < iovs[i].len) {
            break;
        }
    }
    
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fd_write(wasi_fd_t fd, const wasi_ciovec_t* iovs, size_t iovs_len, size_t* nwritten) {
    if (!iovs || !nwritten) {
        return WASI_EINVAL;
    }

    *nwritten = 0;
    
    for (size_t i = 0; i < iovs_len; i++) {
        ssize_t result = write(fd, iovs[i].buf, iovs[i].len);
        if (result < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return WASI_EAGAIN;
            }
            return WASI_EIO;
        }
        *nwritten += result;
        
        // If we wrote less than requested, we're done
        if ((size_t)result < iovs[i].len) {
            break;
        }
    }
    
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fd_seek(wasi_fd_t fd, int64_t offset, uint8_t whence, uint64_t* newoffset) {
    if (!newoffset) {
        return WASI_EINVAL;
    }

    int posix_whence;
    switch (whence) {
        case 0: posix_whence = SEEK_SET; break;
        case 1: posix_whence = SEEK_CUR; break;
        case 2: posix_whence = SEEK_END; break;
        default: return WASI_EINVAL;
    }

    off_t result = lseek(fd, offset, posix_whence);
    if (result < 0) {
        return WASI_EIO;
    }

    *newoffset = (uint64_t)result;
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fd_tell(wasi_fd_t fd, uint64_t* newoffset) {
    if (!newoffset) {
        return WASI_EINVAL;
    }

    off_t result = lseek(fd, 0, SEEK_CUR);
    if (result < 0) {
        return WASI_EIO;
    }

    *newoffset = (uint64_t)result;
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fd_close(wasi_fd_t fd) {
    if (close(fd) < 0) {
        return WASI_EIO;
    }
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fd_fdstat_get(wasi_fd_t fd, wasi_fdstat_t* stat) {
    if (!stat) {
        return WASI_EINVAL;
    }

    // For standard file descriptors, set basic properties
    if (fd == WASI_STDIN_FD || fd == WASI_STDOUT_FD || fd == WASI_STDERR_FD) {
        stat->filetype = WASI_FILETYPE_CHARACTER_DEVICE;
        stat->flags = 0;
        stat->rights_base = WASI_RIGHT_FD_READ | WASI_RIGHT_FD_WRITE;
        stat->rights_inheriting = 0;
        return WASI_ESUCCESS;
    }

    // For other file descriptors, we'd need to query the actual file
    // This is a simplified implementation
    stat->filetype = WASI_FILETYPE_REGULAR_FILE;
    stat->flags = 0;
    stat->rights_base = WASI_RIGHT_FD_READ | WASI_RIGHT_FD_WRITE;
    stat->rights_inheriting = 0;
    
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fd_filestat_get(wasi_fd_t fd, wasi_filestat_t* stat) {
    if (!stat) {
        return WASI_EINVAL;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        return WASI_EIO;
    }

    stat->filetype = S_ISREG(st.st_mode) ? WASI_FILETYPE_REGULAR_FILE :
                     S_ISDIR(st.st_mode) ? WASI_FILETYPE_DIRECTORY :
                     S_ISCHR(st.st_mode) ? WASI_FILETYPE_CHARACTER_DEVICE :
                     S_ISBLK(st.st_mode) ? WASI_FILETYPE_BLOCK_DEVICE :
                     S_ISLNK(st.st_mode) ? WASI_FILETYPE_SYMBOLIC_LINK :
                     WASI_FILETYPE_UNKNOWN;
    
    stat->nlink = st.st_nlink;
    stat->size = st.st_size;
    stat->atim = st.st_atime * 1000000000ULL; // Convert to nanoseconds
    stat->mtim = st.st_mtime * 1000000000ULL;
    stat->ctim = st.st_ctime * 1000000000ULL;
    
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fd_readdir(wasi_fd_t fd, uint8_t* buf, size_t buf_len, uint64_t cookie, size_t* nread) {
    // This is a simplified implementation
    // In a real WASI implementation, this would read directory entries
    *nread = 0;
    return WASI_ENOSYS; // Not implemented in this simplified version
}

wasi_errno_t wasi_path_open(wasi_fd_t dirfd, uint32_t dirflags, const char* path, size_t path_len, 
                           wasi_rights_t rights_base, wasi_rights_t rights_inheriting, 
                           uint16_t fdflags, wasi_fd_t* fd) {
    if (!path || !fd) {
        return WASI_EINVAL;
    }

    // Null-terminate the path
    char* null_terminated_path = malloc(path_len + 1);
    if (!null_terminated_path) {
        return WASI_ENOMEM;
    }
    memcpy(null_terminated_path, path, path_len);
    null_terminated_path[path_len] = '\0';

    int flags = 0;
    if (rights_base & WASI_RIGHT_FD_READ) {
        flags |= O_RDONLY;
    }
    if (rights_base & WASI_RIGHT_FD_WRITE) {
        flags |= O_WRONLY;
    }
    if ((rights_base & WASI_RIGHT_FD_READ) && (rights_base & WASI_RIGHT_FD_WRITE)) {
        flags = O_RDWR;
    }

    int result = open(null_terminated_path, flags);
    free(null_terminated_path);

    if (result < 0) {
        switch (errno) {
            case ENOENT: return WASI_ENOENT;
            case EACCES: return WASI_EACCES;
            case EISDIR: return WASI_EISDIR;
            case ENAMETOOLONG: return WASI_ENAMETOOLONG;
            default: return WASI_EIO;
        }
    }

    *fd = result;
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_clock_time_get(wasi_clockid_t clock_id, wasi_timestamp_t precision, wasi_timestamp_t* time) {
    if (!time) {
        return WASI_EINVAL;
    }

    struct timespec ts;
    int clock_type;
    
    switch (clock_id) {
        case WASI_CLOCK_REALTIME:
            clock_type = CLOCK_REALTIME;
            break;
        case WASI_CLOCK_MONOTONIC:
            clock_type = CLOCK_MONOTONIC;
            break;
        case WASI_CLOCK_PROCESS_CPUTIME_ID:
            clock_type = CLOCK_PROCESS_CPUTIME_ID;
            break;
        case WASI_CLOCK_THREAD_CPUTIME_ID:
            clock_type = CLOCK_THREAD_CPUTIME_ID;
            break;
        default:
            return WASI_EINVAL;
    }

    if (clock_gettime(clock_type, &ts) < 0) {
        return WASI_EIO;
    }

    *time = (wasi_timestamp_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_environ_sizes_get(size_t* environ_count, size_t* environ_size) {
    if (!environ_count || !environ_size) {
        return WASI_EINVAL;
    }

    extern char** environ;
    size_t count = 0;
    size_t size = 0;

    for (char** env = environ; *env; env++) {
        count++;
        size += strlen(*env) + 1; // +1 for null terminator
    }

    *environ_count = count;
    *environ_size = size;
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_environ_get(char** environ, size_t environ_buf_size) {
    if (!environ) {
        return WASI_EINVAL;
    }

    extern char** environ;
    size_t offset = 0;
    size_t count = 0;

    for (char** env = environ; *env; env++) {
        size_t len = strlen(*env) + 1;
        if (offset + len > environ_buf_size) {
            return WASI_ENOMEM;
        }
        
        memcpy((char*)environ + offset, *env, len);
        environ[count] = (char*)environ + offset;
        offset += len;
        count++;
    }

    return WASI_ESUCCESS;
}

wasi_errno_t wasi_args_sizes_get(size_t* argc, size_t* argv_buf_size) {
    if (!argc || !argv_buf_size) {
        return WASI_EINVAL;
    }

    extern int __argc;
    extern char** __argv;
    
    *argc = __argc;
    *argv_buf_size = 0;
    
    for (int i = 0; i < __argc; i++) {
        *argv_buf_size += strlen(__argv[i]) + 1;
    }

    return WASI_ESUCCESS;
}

wasi_errno_t wasi_args_get(char** argv, char* argv_buf) {
    if (!argv || !argv_buf) {
        return WASI_EINVAL;
    }

    extern int __argc;
    extern char** __argv;
    
    size_t offset = 0;
    for (int i = 0; i < __argc; i++) {
        size_t len = strlen(__argv[i]) + 1;
        memcpy(argv_buf + offset, __argv[i], len);
        argv[i] = argv_buf + offset;
        offset += len;
    }

    return WASI_ESUCCESS;
}

wasi_errno_t wasi_random_get(uint8_t* buf, size_t buf_len) {
    if (!buf) {
        return WASI_EINVAL;
    }

    // Use /dev/urandom if available, otherwise fall back to a simple PRNG
    FILE* urandom = fopen("/dev/urandom", "rb");
    if (urandom) {
        size_t nread = fread(buf, 1, buf_len, urandom);
        fclose(urandom);
        if (nread == buf_len) {
            return WASI_ESUCCESS;
        }
    }

    // Fallback to a simple PRNG (not cryptographically secure)
    static uint32_t seed = 12345;
    for (size_t i = 0; i < buf_len; i++) {
        seed = seed * 1103515245 + 12345;
        buf[i] = (uint8_t)(seed >> 16);
    }

    return WASI_ESUCCESS;
}

void wasi_proc_exit(uint32_t exit_code) {
    exit(exit_code);
}
