/**
 * Extension Manager Header
 * Manages PHP extensions and polyfills for WebAssembly
 */

#ifndef EXTENSION_MANAGER_H
#define EXTENSION_MANAGER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Extension types
typedef enum {
    EXT_TYPE_CORE,
    EXT_TYPE_STANDARD,
    EXT_TYPE_POLYFILL
} extension_type_t;

// Extension status
typedef enum {
    EXT_STATUS_DISABLED,
    EXT_STATUS_ENABLED,
    EXT_STATUS_ERROR
} extension_status_t;

// Extension information
typedef struct {
    char* name;
    char* version;
    extension_type_t type;
    extension_status_t status;
    bool (*init_func)(void);
    void (*cleanup_func)(void);
} extension_info_t;

// Extension function registration
typedef struct {
    char* name;
    void* function_ptr;
    int min_args;
    int max_args;
} extension_function_t;

// Extension manager functions
bool extension_manager_init(void);
void extension_manager_cleanup(void);

// Extension management
bool extension_register(const extension_info_t* info);
bool extension_enable(const char* name);
bool extension_disable(const char* name);
extension_status_t extension_get_status(const char* name);

// Function registration
bool extension_register_function(const char* ext_name, const extension_function_t* func);
void* extension_get_function(const char* ext_name, const char* func_name);

// Built-in extensions
bool ext_curl_init(void);
void ext_curl_cleanup(void);

bool ext_mbstring_init(void);
void ext_mbstring_cleanup(void);

bool ext_json_init(void);
void ext_json_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // EXTENSION_MANAGER_H
