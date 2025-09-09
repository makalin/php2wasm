/**
 * WASI I/O Implementation
 * Input/Output operations for WebAssembly
 */

#include "wasi_shim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Standard I/O operations
wasi_errno_t wasi_io_read_stdin(char* buf, size_t count, size_t* nread) {
    return wasi_fd_read(WASI_STDIN_FD, (wasi_iovec_t[]){{(const uint8_t*)buf, count}}, 1, nread);
}

wasi_errno_t wasi_io_write_stdout(const char* str) {
    if (!str) return WASI_EINVAL;
    
    size_t len = strlen(str);
    size_t nwritten;
    return wasi_fd_write(WASI_STDOUT_FD, (wasi_ciovec_t[]){{(uint8_t*)str, len}}, 1, &nwritten);
}

wasi_errno_t wasi_io_write_stderr(const char* str) {
    if (!str) return WASI_EINVAL;
    
    size_t len = strlen(str);
    size_t nwritten;
    return wasi_fd_write(WASI_STDERR_FD, (wasi_ciovec_t[]){{(uint8_t*)str, len}}, 1, &nwritten);
}

wasi_errno_t wasi_io_printf(const char* format, ...) {
    if (!format) return WASI_EINVAL;
    
    va_list args;
    va_start(args, format);
    
    char buffer[1024];
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (len < 0) return WASI_EINVAL;
    if (len >= sizeof(buffer)) return WASI_ENOMEM;
    
    return wasi_io_write_stdout(buffer);
}

wasi_errno_t wasi_io_fprintf(wasi_fd_t fd, const char* format, ...) {
    if (!format) return WASI_EINVAL;
    
    va_list args;
    va_start(args, format);
    
    char buffer[1024];
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (len < 0) return WASI_EINVAL;
    if (len >= sizeof(buffer)) return WASI_ENOMEM;
    
    size_t nwritten;
    return wasi_fd_write(fd, (wasi_ciovec_t[]){{(uint8_t*)buffer, len}}, 1, &nwritten);
}
