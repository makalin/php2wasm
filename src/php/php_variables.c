/**
 * PHP Variables Implementation
 * Variable management and scope handling
 */

#include "php_engine.h"
#include <stdlib.h>
#include <string.h>

// Variable scope
typedef enum {
    SCOPE_GLOBAL,
    SCOPE_LOCAL,
    SCOPE_FUNCTION
} variable_scope_t;

// Variable entry
typedef struct variable_entry {
    char* name;
    php_value_t* value;
    variable_scope_t scope;
    struct variable_entry* next;
} variable_entry_t;

// Variable table
typedef struct {
    variable_entry_t* entries;
    size_t count;
    size_t capacity;
} variable_table_t;

static variable_table_t global_variables = {0};
static variable_table_t* current_scope = &global_variables;

// Initialize variable system
bool php_variables_init(void) {
    global_variables.capacity = 64;
    global_variables.entries = calloc(global_variables.capacity, sizeof(variable_entry_t));
    if (!global_variables.entries) {
        return false;
    }
    return true;
}

// Cleanup variable system
void php_variables_cleanup(void) {
    for (size_t i = 0; i < global_variables.count; i++) {
        if (global_variables.entries[i].name) {
            free(global_variables.entries[i].name);
        }
        if (global_variables.entries[i].value) {
            php_value_destroy(global_variables.entries[i].value);
        }
    }
    free(global_variables.entries);
    global_variables.entries = NULL;
    global_variables.count = 0;
    global_variables.capacity = 0;
}

// Set variable
bool php_variable_set(const char* name, php_value_t* value) {
    if (!name || !value) return false;
    
    // Check if variable already exists
    for (size_t i = 0; i < current_scope->count; i++) {
        if (current_scope->entries[i].name && 
            strcmp(current_scope->entries[i].name, name) == 0) {
            // Update existing variable
            php_value_destroy(current_scope->entries[i].value);
            current_scope->entries[i].value = value;
            return true;
        }
    }
    
    // Add new variable
    if (current_scope->count >= current_scope->capacity) {
        current_scope->capacity *= 2;
        current_scope->entries = realloc(current_scope->entries, 
                                        current_scope->capacity * sizeof(variable_entry_t));
        if (!current_scope->entries) {
            return false;
        }
    }
    
    current_scope->entries[current_scope->count].name = strdup(name);
    current_scope->entries[current_scope->count].value = value;
    current_scope->entries[current_scope->count].scope = SCOPE_GLOBAL;
    current_scope->count++;
    
    return true;
}

// Get variable
php_value_t* php_variable_get(const char* name) {
    if (!name) return NULL;
    
    // Search current scope first
    for (size_t i = 0; i < current_scope->count; i++) {
        if (current_scope->entries[i].name && 
            strcmp(current_scope->entries[i].name, name) == 0) {
            return current_scope->entries[i].value;
        }
    }
    
    return NULL;
}

// Unset variable
bool php_variable_unset(const char* name) {
    if (!name) return false;
    
    for (size_t i = 0; i < current_scope->count; i++) {
        if (current_scope->entries[i].name && 
            strcmp(current_scope->entries[i].name, name) == 0) {
            free(current_scope->entries[i].name);
            php_value_destroy(current_scope->entries[i].value);
            
            // Move last entry to this position
            if (i < current_scope->count - 1) {
                current_scope->entries[i] = current_scope->entries[current_scope->count - 1];
            }
            current_scope->count--;
            return true;
        }
    }
    
    return false;
}

// Check if variable is set
bool php_variable_isset(const char* name) {
    return php_variable_get(name) != NULL;
}

// Check if variable is empty
bool php_variable_empty(const char* name) {
    php_value_t* value = php_variable_get(name);
    if (!value) return true;
    
    switch (value->type) {
        case PHP_TYPE_NULL:
            return true;
        case PHP_TYPE_BOOL:
            return !value->value.bool_val;
        case PHP_TYPE_INT:
            return value->value.int_val == 0;
        case PHP_TYPE_FLOAT:
            return value->value.float_val == 0.0;
        case PHP_TYPE_STRING:
            return strlen(value->value.string_val) == 0;
        case PHP_TYPE_ARRAY:
            return true; // Simplified - would check array length
        default:
            return false;
    }
}
