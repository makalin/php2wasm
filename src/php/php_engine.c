/**
 * PHP Engine Implementation
 * Core PHP runtime engine for WebAssembly
 */

#include "php_engine.h"
#include "wasi/wasi_shim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Global engine state
static php_engine_state_t engine_state = PHP_ENGINE_UNINITIALIZED;
static php_value_t** global_variables = NULL;
static php_function_t* registered_functions = NULL;
static size_t global_vars_count = 0;
static size_t global_vars_capacity = 0;
static size_t functions_count = 0;
static size_t functions_capacity = 0;

// Forward declarations
static void register_builtin_functions(void);
static php_value_t* parse_php_expression(const char* code, size_t* consumed);

bool php_engine_init(void) {
    if (engine_state != PHP_ENGINE_UNINITIALIZED) {
        return true;
    }

    // Initialize global variables storage
    global_vars_capacity = 64;
    global_variables = calloc(global_vars_capacity, sizeof(php_value_t*));
    if (!global_variables) {
        return false;
    }

    // Initialize functions storage
    functions_capacity = 32;
    registered_functions = calloc(functions_capacity, sizeof(php_function_t));
    if (!registered_functions) {
        free(global_variables);
        return false;
    }

    // Register built-in functions
    register_builtin_functions();

    engine_state = PHP_ENGINE_INITIALIZED;
    return true;
}

void php_engine_cleanup(void) {
    if (engine_state == PHP_ENGINE_UNINITIALIZED) {
        return;
    }

    // Clean up global variables
    if (global_variables) {
        for (size_t i = 0; i < global_vars_count; i++) {
            if (global_variables[i]) {
                php_value_destroy(global_variables[i]);
            }
        }
        free(global_variables);
        global_variables = NULL;
    }

    // Clean up functions
    if (registered_functions) {
        for (size_t i = 0; i < functions_count; i++) {
            if (registered_functions[i].name) {
                free(registered_functions[i].name);
            }
        }
        free(registered_functions);
        registered_functions = NULL;
    }

    global_vars_count = 0;
    global_vars_capacity = 0;
    functions_count = 0;
    functions_capacity = 0;
    engine_state = PHP_ENGINE_UNINITIALIZED;
}

php_engine_state_t php_engine_get_state(void) {
    return engine_state;
}

bool php_engine_execute_file(const char* filename) {
    if (engine_state != PHP_ENGINE_INITIALIZED) {
        return false;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        php_engine_error("Failed to open file");
        return false;
    }

    // Read file content
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        return false;
    }

    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);

    // Execute content
    bool result = php_engine_execute_string(content);
    free(content);
    return result;
}

bool php_engine_execute_string(const char* code) {
    if (engine_state != PHP_ENGINE_INITIALIZED) {
        return false;
    }

    engine_state = PHP_ENGINE_RUNNING;

    // Simple PHP parser - this is a very basic implementation
    const char* pos = code;
    while (*pos) {
        // Skip whitespace
        while (*pos && (*pos == ' ' || *pos == '\t' || *pos == '\n' || *pos == '\r')) {
            pos++;
        }

        if (!*pos) break;

        // Handle PHP tags
        if (strncmp(pos, "<?php", 5) == 0) {
            pos += 5;
            continue;
        }
        if (strncmp(pos, "<?", 2) == 0) {
            pos += 2;
            continue;
        }
        if (strncmp(pos, "?>", 2) == 0) {
            pos += 2;
            continue;
        }

        // Handle echo statements
        if (strncmp(pos, "echo", 4) == 0) {
            pos += 4;
            while (*pos && (*pos == ' ' || *pos == '\t')) pos++;
            
            if (*pos == '"' || *pos == '\'') {
                char quote = *pos++;
                const char* start = pos;
                while (*pos && *pos != quote) pos++;
                if (*pos == quote) {
                    size_t len = pos - start;
                    char* str = malloc(len + 1);
                    memcpy(str, start, len);
                    str[len] = '\0';
                    php_engine_output(str);
                    free(str);
                    pos++;
                }
            }
            continue;
        }

        // Handle print statements
        if (strncmp(pos, "print", 5) == 0) {
            pos += 5;
            while (*pos && (*pos == ' ' || *pos == '\t')) pos++;
            
            if (*pos == '"' || *pos == '\'') {
                char quote = *pos++;
                const char* start = pos;
                while (*pos && *pos != quote) pos++;
                if (*pos == quote) {
                    size_t len = pos - start;
                    char* str = malloc(len + 1);
                    memcpy(str, start, len);
                    str[len] = '\0';
                    php_engine_output(str);
                    free(str);
                    pos++;
                }
            }
            continue;
        }

        // Skip unknown statements
        while (*pos && *pos != ';' && *pos != '\n') {
            pos++;
        }
        if (*pos == ';') pos++;
    }

    engine_state = PHP_ENGINE_INITIALIZED;
    return true;
}

bool php_engine_syntax_check(const char* filename) {
    // Basic syntax check - in a real implementation, this would be much more comprehensive
    FILE* file = fopen(filename, "r");
    if (!file) {
        return false;
    }

    char line[1024];
    int line_num = 1;
    bool in_php = false;
    int brace_count = 0;
    int paren_count = 0;

    while (fgets(line, sizeof(line), file)) {
        char* pos = line;
        
        // Check for PHP tags
        if (strstr(pos, "<?php") || strstr(pos, "<?=")) {
            in_php = true;
        }
        if (strstr(pos, "?>")) {
            in_php = false;
        }

        if (in_php) {
            // Basic brace/paren matching
            while (*pos) {
                if (*pos == '{') brace_count++;
                else if (*pos == '}') brace_count--;
                else if (*pos == '(') paren_count++;
                else if (*pos == ')') paren_count--;
                pos++;
            }
        }
        line_num++;
    }

    fclose(file);
    return brace_count == 0 && paren_count == 0;
}

// Memory management
php_value_t* php_value_create_null(void) {
    php_value_t* value = malloc(sizeof(php_value_t));
    if (!value) return NULL;
    
    value->type = PHP_TYPE_NULL;
    value->refcount = 1;
    return value;
}

php_value_t* php_value_create_bool(bool val) {
    php_value_t* value = malloc(sizeof(php_value_t));
    if (!value) return NULL;
    
    value->type = PHP_TYPE_BOOL;
    value->value.bool_val = val;
    value->refcount = 1;
    return value;
}

php_value_t* php_value_create_int(int64_t val) {
    php_value_t* value = malloc(sizeof(php_value_t));
    if (!value) return NULL;
    
    value->type = PHP_TYPE_INT;
    value->value.int_val = val;
    value->refcount = 1;
    return value;
}

php_value_t* php_value_create_float(double val) {
    php_value_t* value = malloc(sizeof(php_value_t));
    if (!value) return NULL;
    
    value->type = PHP_TYPE_FLOAT;
    value->value.float_val = val;
    value->refcount = 1;
    return value;
}

php_value_t* php_value_create_string(const char* val) {
    if (!val) return php_value_create_null();
    
    php_value_t* value = malloc(sizeof(php_value_t));
    if (!value) return NULL;
    
    value->type = PHP_TYPE_STRING;
    value->value.string_val = strdup(val);
    value->refcount = 1;
    return value;
}

php_value_t* php_value_create_string_len(const char* val, size_t length) {
    if (!val) return php_value_create_null();
    
    php_value_t* value = malloc(sizeof(php_value_t));
    if (!value) return NULL;
    
    value->type = PHP_TYPE_STRING;
    value->value.string_val = malloc(length + 1);
    memcpy(value->value.string_val, val, length);
    value->value.string_val[length] = '\0';
    value->refcount = 1;
    return value;
}

void php_value_destroy(php_value_t* value) {
    if (!value) return;
    
    if (--value->refcount == 0) {
        if (value->type == PHP_TYPE_STRING && value->value.string_val) {
            free(value->value.string_val);
        }
        free(value);
    }
}

void php_value_ref(php_value_t* value) {
    if (value) {
        value->refcount++;
    }
}

void php_value_unref(php_value_t* value) {
    php_value_destroy(value);
}

// Variable management
bool php_engine_set_variable(const char* name, php_value_t* value) {
    if (!name || !value) return false;
    
    // Find existing variable
    for (size_t i = 0; i < global_vars_count; i++) {
        if (global_variables[i] && strcmp(global_variables[i]->value.string_val, name) == 0) {
            php_value_destroy(global_variables[i]);
            global_variables[i] = value;
            return true;
        }
    }
    
    // Add new variable
    if (global_vars_count >= global_vars_capacity) {
        global_vars_capacity *= 2;
        global_variables = realloc(global_variables, global_vars_capacity * sizeof(php_value_t*));
        if (!global_variables) return false;
    }
    
    global_variables[global_vars_count++] = value;
    return true;
}

php_value_t* php_engine_get_variable(const char* name) {
    if (!name) return NULL;
    
    for (size_t i = 0; i < global_vars_count; i++) {
        if (global_variables[i] && strcmp(global_variables[i]->value.string_val, name) == 0) {
            return global_variables[i];
        }
    }
    
    return NULL;
}

// Output functions
void php_engine_output(const char* str) {
    if (!str) return;
    php_engine_output_len(str, strlen(str));
}

void php_engine_output_len(const char* str, size_t length) {
    if (!str) return;
    
    wasi_ciovec_t iov = {(uint8_t*)str, length};
    size_t nwritten;
    wasi_fd_write(WASI_STDOUT_FD, &iov, 1, &nwritten);
}

void php_engine_output_int(int64_t value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lld", (long long)value);
    php_engine_output(buffer);
}

void php_engine_output_float(double value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.6g", value);
    php_engine_output(buffer);
}

void php_engine_output_bool(bool value) {
    php_engine_output(value ? "1" : "");
}

// Error handling
void php_engine_error(const char* message) {
    if (!message) return;
    
    wasi_ciovec_t iov = {(uint8_t*)message, strlen(message)};
    size_t nwritten;
    wasi_fd_write(WASI_STDERR_FD, &iov, 1, &nwritten);
}

void php_engine_warning(const char* message) {
    php_engine_error(message);
}

void php_engine_notice(const char* message) {
    php_engine_error(message);
}

// Built-in function implementations
php_value_t* php_function_echo(int argc, php_value_t** argv) {
    for (int i = 0; i < argc; i++) {
        if (argv[i]) {
            switch (argv[i]->type) {
                case PHP_TYPE_STRING:
                    php_engine_output(argv[i]->value.string_val);
                    break;
                case PHP_TYPE_INT:
                    php_engine_output_int(argv[i]->value.int_val);
                    break;
                case PHP_TYPE_FLOAT:
                    php_engine_output_float(argv[i]->value.float_val);
                    break;
                case PHP_TYPE_BOOL:
                    php_engine_output_bool(argv[i]->value.bool_val);
                    break;
                default:
                    php_engine_output("NULL");
                    break;
            }
        }
    }
    return php_value_create_null();
}

php_value_t* php_function_print(int argc, php_value_t** argv) {
    if (argc > 0 && argv[0]) {
        php_function_echo(1, argv);
    }
    return php_value_create_int(1);
}

php_value_t* php_function_strlen(int argc, php_value_t** argv) {
    if (argc < 1 || !argv[0] || argv[0]->type != PHP_TYPE_STRING) {
        return php_value_create_int(0);
    }
    
    return php_value_create_int(strlen(argv[0]->value.string_val));
}

// Register built-in functions
static void register_builtin_functions(void) {
    php_function_t functions[] = {
        {"echo", php_function_echo, 1, -1},
        {"print", php_function_print, 1, 1},
        {"strlen", php_function_strlen, 1, 1},
        {NULL, NULL, 0, 0}
    };
    
    for (int i = 0; functions[i].name; i++) {
        php_engine_register_function(&functions[i]);
    }
}

bool php_engine_register_function(const php_function_t* func) {
    if (!func || !func->name || !func->callback) {
        return false;
    }
    
    if (functions_count >= functions_capacity) {
        functions_capacity *= 2;
        registered_functions = realloc(registered_functions, functions_capacity * sizeof(php_function_t));
        if (!registered_functions) return false;
    }
    
    registered_functions[functions_count].name = strdup(func->name);
    registered_functions[functions_count].callback = func->callback;
    registered_functions[functions_count].min_args = func->min_args;
    registered_functions[functions_count].max_args = func->max_args;
    functions_count++;
    
    return true;
}

php_value_t* php_engine_call_function(const char* name, int argc, php_value_t** argv) {
    if (!name) return NULL;
    
    for (size_t i = 0; i < functions_count; i++) {
        if (strcmp(registered_functions[i].name, name) == 0) {
            if (argc < registered_functions[i].min_args || 
                (registered_functions[i].max_args > 0 && argc > registered_functions[i].max_args)) {
                return NULL;
            }
            return registered_functions[i].callback(argc, argv);
        }
    }
    
    return NULL;
}
