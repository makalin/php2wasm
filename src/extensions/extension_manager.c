/**
 * Extension Manager Implementation
 * Manages PHP extensions and polyfills for WebAssembly
 */

#include "extension_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global extension registry
static extension_info_t* extensions = NULL;
static size_t extensions_count = 0;
static size_t extensions_capacity = 0;

// Global function registry
static extension_function_t* functions = NULL;
static size_t functions_count = 0;
static size_t functions_capacity = 0;

bool extension_manager_init(void) {
    // Initialize extension registry
    extensions_capacity = 16;
    extensions = calloc(extensions_capacity, sizeof(extension_info_t));
    if (!extensions) {
        return false;
    }

    // Initialize function registry
    functions_capacity = 64;
    functions = calloc(functions_capacity, sizeof(extension_function_t));
    if (!functions) {
        free(extensions);
        return false;
    }

    // Register built-in extensions
    extension_info_t builtin_extensions[] = {
        {
            .name = "curl",
            .version = "1.0.0",
            .type = EXT_TYPE_POLYFILL,
            .status = EXT_STATUS_DISABLED,
            .init_func = ext_curl_init,
            .cleanup_func = ext_curl_cleanup
        },
        {
            .name = "mbstring",
            .version = "1.0.0",
            .type = EXT_TYPE_POLYFILL,
            .status = EXT_STATUS_DISABLED,
            .init_func = ext_mbstring_init,
            .cleanup_func = ext_mbstring_cleanup
        },
        {
            .name = "json",
            .version = "1.0.0",
            .type = EXT_TYPE_CORE,
            .status = EXT_STATUS_ENABLED,
            .init_func = ext_json_init,
            .cleanup_func = ext_json_cleanup
        }
    };

    for (int i = 0; i < 3; i++) {
        if (!extension_register(&builtin_extensions[i])) {
            extension_manager_cleanup();
            return false;
        }
    }

    return true;
}

void extension_manager_cleanup(void) {
    // Cleanup all extensions
    for (size_t i = 0; i < extensions_count; i++) {
        if (extensions[i].name) {
            free(extensions[i].name);
        }
        if (extensions[i].version) {
            free(extensions[i].version);
        }
        if (extensions[i].status == EXT_STATUS_ENABLED && extensions[i].cleanup_func) {
            extensions[i].cleanup_func();
        }
    }

    if (extensions) {
        free(extensions);
        extensions = NULL;
    }

    // Cleanup function registry
    for (size_t i = 0; i < functions_count; i++) {
        if (functions[i].name) {
            free(functions[i].name);
        }
    }

    if (functions) {
        free(functions);
        functions = NULL;
    }

    extensions_count = 0;
    extensions_capacity = 0;
    functions_count = 0;
    functions_capacity = 0;
}

bool extension_register(const extension_info_t* info) {
    if (!info || !info->name) {
        return false;
    }

    // Check if extension already exists
    for (size_t i = 0; i < extensions_count; i++) {
        if (strcmp(extensions[i].name, info->name) == 0) {
            return false; // Extension already registered
        }
    }

    // Resize if needed
    if (extensions_count >= extensions_capacity) {
        extensions_capacity *= 2;
        extensions = realloc(extensions, extensions_capacity * sizeof(extension_info_t));
        if (!extensions) {
            return false;
        }
    }

    // Register extension
    extensions[extensions_count].name = strdup(info->name);
    extensions[extensions_count].version = strdup(info->version);
    extensions[extensions_count].type = info->type;
    extensions[extensions_count].status = info->status;
    extensions[extensions_count].init_func = info->init_func;
    extensions[extensions_count].cleanup_func = info->cleanup_func;

    extensions_count++;
    return true;
}

bool extension_enable(const char* name) {
    if (!name) {
        return false;
    }

    for (size_t i = 0; i < extensions_count; i++) {
        if (strcmp(extensions[i].name, name) == 0) {
            if (extensions[i].status == EXT_STATUS_ENABLED) {
                return true; // Already enabled
            }

            if (extensions[i].init_func && !extensions[i].init_func()) {
                extensions[i].status = EXT_STATUS_ERROR;
                return false;
            }

            extensions[i].status = EXT_STATUS_ENABLED;
            return true;
        }
    }

    return false; // Extension not found
}

bool extension_disable(const char* name) {
    if (!name) {
        return false;
    }

    for (size_t i = 0; i < extensions_count; i++) {
        if (strcmp(extensions[i].name, name) == 0) {
            if (extensions[i].status == EXT_STATUS_DISABLED) {
                return true; // Already disabled
            }

            if (extensions[i].cleanup_func) {
                extensions[i].cleanup_func();
            }

            extensions[i].status = EXT_STATUS_DISABLED;
            return true;
        }
    }

    return false; // Extension not found
}

extension_status_t extension_get_status(const char* name) {
    if (!name) {
        return EXT_STATUS_ERROR;
    }

    for (size_t i = 0; i < extensions_count; i++) {
        if (strcmp(extensions[i].name, name) == 0) {
            return extensions[i].status;
        }
    }

    return EXT_STATUS_ERROR; // Extension not found
}

bool extension_register_function(const char* ext_name, const extension_function_t* func) {
    if (!ext_name || !func || !func->name || !func->function_ptr) {
        return false;
    }

    // Check if extension exists
    bool ext_exists = false;
    for (size_t i = 0; i < extensions_count; i++) {
        if (strcmp(extensions[i].name, ext_name) == 0) {
            ext_exists = true;
            break;
        }
    }

    if (!ext_exists) {
        return false;
    }

    // Resize if needed
    if (functions_count >= functions_capacity) {
        functions_capacity *= 2;
        functions = realloc(functions, functions_capacity * sizeof(extension_function_t));
        if (!functions) {
            return false;
        }
    }

    // Register function
    functions[functions_count].name = strdup(func->name);
    functions[functions_count].function_ptr = func->function_ptr;
    functions[functions_count].min_args = func->min_args;
    functions[functions_count].max_args = func->max_args;

    functions_count++;
    return true;
}

void* extension_get_function(const char* ext_name, const char* func_name) {
    if (!ext_name || !func_name) {
        return NULL;
    }

    // This is a simplified lookup - in a real implementation,
    // you'd want to organize functions by extension
    for (size_t i = 0; i < functions_count; i++) {
        if (strcmp(functions[i].name, func_name) == 0) {
            return functions[i].function_ptr;
        }
    }

    return NULL;
}

// Built-in extension implementations

bool ext_curl_init(void) {
    // Initialize curl polyfill
    // This would set up HTTP client functionality using WASI networking
    return true;
}

void ext_curl_cleanup(void) {
    // Cleanup curl polyfill
}

bool ext_mbstring_init(void) {
    // Initialize mbstring polyfill
    // This would set up multibyte string functions
    return true;
}

void ext_mbstring_cleanup(void) {
    // Cleanup mbstring polyfill
}

bool ext_json_init(void) {
    // JSON is built-in, just mark as initialized
    return true;
}

void ext_json_cleanup(void) {
    // JSON cleanup (nothing to do)
}
