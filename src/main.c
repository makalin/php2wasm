/**
 * php2wasm - Main entry point
 * PHP to WebAssembly compiler with WASI support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "wasi/wasi_shim.h"
#include "php/php_engine.h"
#include "extensions/extension_manager.h"

static void print_usage(const char* program_name) {
    printf("Usage: %s [options] <file> [args...]\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -v, --version  Show version information\n");
    printf("  -d key=value   Set php.ini directive\n");
    printf("  -e             Evaluate code from command line\n");
    printf("  -r             Run code from command line\n");
    printf("  -f             Parse and execute file\n");
    printf("  -l             Syntax check only\n");
    printf("  -s             Output HTML syntax highlighted source\n");
    printf("  -w             Strip whitespace and comments\n");
    printf("  -z             Load Zend extension\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s script.php\n", program_name);
    printf("  %s -r 'echo \"Hello World\";'\n", program_name);
    printf("  %s -d display_errors=1 script.php\n", program_name);
}

static void print_version(void) {
    printf("PHP %s (WASI) (built: %s %s)\n", PHP_VERSION, __DATE__, __TIME__);
    printf("Copyright (c) 1997-2024 The PHP Group\n");
    printf("Copyright (c) 2024 Mehmet T. AKALIN (php2wasm)\n");
    printf("Zend Engine v%s, with php2wasm v1.0.0\n", ZEND_VERSION);
}

int main(int argc, char* argv[]) {
    // Initialize WASI
    if (!wasi_init()) {
        fprintf(stderr, "Failed to initialize WASI\n");
        return 1;
    }

    // Initialize PHP engine
    if (!php_engine_init()) {
        fprintf(stderr, "Failed to initialize PHP engine\n");
        return 1;
    }

    // Initialize extensions
    if (!extension_manager_init()) {
        fprintf(stderr, "Failed to initialize extensions\n");
        return 1;
    }

    // Parse command line arguments
    int opt;
    char* script_file = NULL;
    char* eval_code = NULL;
    int syntax_check = 0;
    int html_syntax = 0;
    int strip_whitespace = 0;

    while ((opt = getopt(argc, argv, "hvdef:lrs:wz:")) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'v':
                print_version();
                return 0;
            case 'd':
                // TODO: Handle php.ini directives
                break;
            case 'e':
            case 'r':
                if (optind < argc) {
                    eval_code = argv[optind];
                    optind++;
                } else {
                    fprintf(stderr, "Code required for -%c option\n", opt);
                    return 1;
                }
                break;
            case 'f':
                if (optind < argc) {
                    script_file = argv[optind];
                    optind++;
                } else {
                    fprintf(stderr, "File required for -f option\n");
                    return 1;
                }
                break;
            case 'l':
                syntax_check = 1;
                break;
            case 's':
                html_syntax = 1;
                break;
            case 'w':
                strip_whitespace = 1;
                break;
            case 'z':
                // TODO: Handle Zend extensions
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Determine what to execute
    if (eval_code) {
        // Execute code from command line
        if (!php_engine_execute_string(eval_code)) {
            fprintf(stderr, "Failed to execute code\n");
            return 1;
        }
    } else if (script_file) {
        // Execute script file
        if (syntax_check) {
            if (!php_engine_syntax_check(script_file)) {
                fprintf(stderr, "Syntax error in %s\n", script_file);
                return 1;
            }
            printf("No syntax errors detected in %s\n", script_file);
        } else {
            if (!php_engine_execute_file(script_file)) {
                fprintf(stderr, "Failed to execute %s\n", script_file);
                return 1;
            }
        }
    } else if (optind < argc) {
        // Execute script file (positional argument)
        script_file = argv[optind];
        if (syntax_check) {
            if (!php_engine_syntax_check(script_file)) {
                fprintf(stderr, "Syntax error in %s\n", script_file);
                return 1;
            }
            printf("No syntax errors detected in %s\n", script_file);
        } else {
            if (!php_engine_execute_file(script_file)) {
                fprintf(stderr, "Failed to execute %s\n", script_file);
                return 1;
            }
        }
    } else {
        // No input specified, show usage
        print_usage(argv[0]);
        return 1;
    }

    // Cleanup
    extension_manager_cleanup();
    php_engine_cleanup();
    wasi_cleanup();

    return 0;
}
