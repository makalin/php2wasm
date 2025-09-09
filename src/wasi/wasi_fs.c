/**
 * WASI File System Implementation
 * File system operations for WebAssembly
 */

#include "wasi_shim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// File system operations
wasi_errno_t wasi_fs_open(const char* path, int flags, wasi_fd_t* fd) {
    if (!path || !fd) {
        return WASI_EINVAL;
    }
    
    int posix_flags = 0;
    if (flags & O_RDONLY) posix_flags |= O_RDONLY;
    if (flags & O_WRONLY) posix_flags |= O_WRONLY;
    if (flags & O_RDWR) posix_flags |= O_RDWR;
    if (flags & O_CREAT) posix_flags |= O_CREAT;
    if (flags & O_TRUNC) posix_flags |= O_TRUNC;
    if (flags & O_APPEND) posix_flags |= O_APPEND;
    
    int result = open(path, posix_flags, 0644);
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

wasi_errno_t wasi_fs_close(wasi_fd_t fd) {
    if (close(fd) < 0) {
        return WASI_EIO;
    }
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fs_read(wasi_fd_t fd, void* buf, size_t count, size_t* nread) {
    if (!buf || !nread) {
        return WASI_EINVAL;
    }
    
    ssize_t result = read(fd, buf, count);
    if (result < 0) {
        return WASI_EIO;
    }
    
    *nread = result;
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fs_write(wasi_fd_t fd, const void* buf, size_t count, size_t* nwritten) {
    if (!buf || !nwritten) {
        return WASI_EINVAL;
    }
    
    ssize_t result = write(fd, buf, count);
    if (result < 0) {
        return WASI_EIO;
    }
    
    *nwritten = result;
    return WASI_ESUCCESS;
}

wasi_errno_t wasi_fs_stat(const char* path, wasi_filestat_t* stat) {
    if (!path || !stat) {
        return WASI_EINVAL;
    }
    
    struct stat st;
    if (stat(path, &st) < 0) {
        switch (errno) {
            case ENOENT: return WASI_ENOENT;
            case EACCES: return WASI_EACCES;
            default: return WASI_EIO;
        }
    }
    
    stat->filetype = S_ISREG(st.st_mode) ? WASI_FILETYPE_REGULAR_FILE :
                     S_ISDIR(st.st_mode) ? WASI_FILETYPE_DIRECTORY :
                     S_ISCHR(st.st_mode) ? WASI_FILETYPE_CHARACTER_DEVICE :
                     S_ISBLK(st.st_mode) ? WASI_FILETYPE_BLOCK_DEVICE :
                     S_ISLNK(st.st_mode) ? WASI_FILETYPE_SYMBOLIC_LINK :
                     WASI_FILETYPE_UNKNOWN;
    
    stat->nlink = st.st_nlink;
    stat->size = st.st_size;
    stat->atim = st.st_atime * 1000000000ULL;
    stat->mtim = st.st_mtime * 1000000000ULL;
    stat->ctim = st.st_ctime * 1000000000ULL;
    
    return WASI_ESUCCESS;
}
