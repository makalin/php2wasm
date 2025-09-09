/**
 * PHP Engine Header
 * Core PHP runtime engine for WebAssembly
 */

#ifndef PHP_ENGINE_H
#define PHP_ENGINE_H

#include <stdbool.h>
#include <stdint.h>

// PHP version constants
#define PHP_VERSION "8.3.0"
#define ZEND_VERSION "4.3.0"

// PHP engine states
typedef enum {
    PHP_ENGINE_UNINITIALIZED,
    PHP_ENGINE_INITIALIZED,
    PHP_ENGINE_RUNNING,
    PHP_ENGINE_ERROR
} php_engine_state_t;

// PHP value types
typedef enum {
    PHP_TYPE_NULL,
    PHP_TYPE_BOOL,
    PHP_TYPE_INT,
    PHP_TYPE_FLOAT,
    PHP_TYPE_STRING,
    PHP_TYPE_ARRAY,
    PHP_TYPE_OBJECT,
    PHP_TYPE_RESOURCE
} php_type_t;

// PHP value structure
typedef struct {
    php_type_t type;
    union {
        bool bool_val;
        int64_t int_val;
        double float_val;
        char* string_val;
        void* array_val;
        void* object_val;
        void* resource_val;
    } value;
    uint32_t refcount;
} php_value_t;

// PHP function callback
typedef php_value_t* (*php_function_callback_t)(int argc, php_value_t** argv);

// PHP function registration
typedef struct {
    char* name;
    php_function_callback_t callback;
    int min_args;
    int max_args;
} php_function_t;

// Engine initialization and cleanup
bool php_engine_init(void);
void php_engine_cleanup(void);
php_engine_state_t php_engine_get_state(void);

// Code execution
bool php_engine_execute_file(const char* filename);
bool php_engine_execute_string(const char* code);
bool php_engine_syntax_check(const char* filename);

// Memory management
php_value_t* php_value_create_null(void);
php_value_t* php_value_create_bool(bool value);
php_value_t* php_value_create_int(int64_t value);
php_value_t* php_value_create_float(double value);
php_value_t* php_value_create_string(const char* value);
php_value_t* php_value_create_string_len(const char* value, size_t length);

void php_value_destroy(php_value_t* value);
void php_value_ref(php_value_t* value);
void php_value_unref(php_value_t* value);

// Variable management
bool php_engine_set_variable(const char* name, php_value_t* value);
php_value_t* php_engine_get_variable(const char* name);
bool php_engine_unset_variable(const char* name);

// Function management
bool php_engine_register_function(const php_function_t* func);
php_value_t* php_engine_call_function(const char* name, int argc, php_value_t** argv);

// Output functions
void php_engine_output(const char* str);
void php_engine_output_len(const char* str, size_t length);
void php_engine_output_int(int64_t value);
void php_engine_output_float(double value);
void php_engine_output_bool(bool value);

// Error handling
void php_engine_error(const char* message);
void php_engine_warning(const char* message);
void php_engine_notice(const char* message);

// Built-in functions
php_value_t* php_function_echo(int argc, php_value_t** argv);
php_value_t* php_function_print(int argc, php_value_t** argv);
php_value_t* php_function_var_dump(int argc, php_value_t** argv);
php_value_t* php_function_count(int argc, php_value_t** argv);
php_value_t* php_function_strlen(int argc, php_value_t** argv);
php_value_t* php_function_strpos(int argc, php_value_t** argv);
php_value_t* php_function_substr(int argc, php_value_t** argv);
php_value_t* php_function_trim(int argc, php_value_t** argv);
php_value_t* php_function_strtolower(int argc, php_value_t** argv);
php_value_t* php_function_strtoupper(int argc, php_value_t** argv);
php_value_t* php_function_array_push(int argc, php_value_t** argv);
php_value_t* php_function_array_pop(int argc, php_value_t** argv);
php_value_t* php_function_array_keys(int argc, php_value_t** argv);
php_value_t* php_function_array_values(int argc, php_value_t** argv);
php_value_t* php_function_array_merge(int argc, php_value_t** argv);
php_value_t* php_function_in_array(int argc, php_value_t** argv);
php_value_t* php_function_array_key_exists(int argc, php_value_t** argv);
php_value_t* php_function_is_array(int argc, php_value_t** argv);
php_value_t* php_function_is_string(int argc, php_value_t** argv);
php_value_t* php_function_is_int(int argc, php_value_t** argv);
php_value_t* php_function_is_float(int argc, php_value_t** argv);
php_value_t* php_function_is_bool(int argc, php_value_t** argv);
php_value_t* php_function_is_null(int argc, php_value_t** argv);
php_value_t* php_function_gettype(int argc, php_value_t** argv);
php_value_t* php_function_isset(int argc, php_value_t** argv);
php_value_t* php_function_unset(int argc, php_value_t** argv);
php_value_t* php_function_empty(int argc, php_value_t** argv);
php_value_t* php_function_exit(int argc, php_value_t** argv);
php_value_t* php_function_die(int argc, php_value_t** argv);

#ifdef __cplusplus
}
#endif

#endif // PHP_ENGINE_H
