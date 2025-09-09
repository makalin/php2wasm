cmake_minimum_required(VERSION 3.16)
project(php2wasm VERSION 1.0.0 LANGUAGES C CXX)

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# WASI specific flags
if(CMAKE_SYSTEM_NAME STREQUAL "WASI")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_WASI_EMULATED_PROCESS_CLOCKS -D_WASI_EMULATED_SIGNAL")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WASI_EMULATED_PROCESS_CLOCKS -D_WASI_EMULATED_SIGNAL")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-entry -Wl,--export-dynamic")
endif()

# PHP version
if(NOT DEFINED PHP_VERSION)
    set(PHP_VERSION "8.3.0")
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/php
    ${CMAKE_SOURCE_DIR}/src/wasi
    ${CMAKE_SOURCE_DIR}/src/extensions
)

# Source files
set(PHP2WASM_SOURCES
    src/main.c
    src/wasi/wasi_shim.c
    src/wasi/wasi_fs.c
    src/wasi/wasi_io.c
    src/php/php_engine.c
    src/php/php_parser.c
    src/php/php_executor.c
    src/php/php_memory.c
    src/php/php_variables.c
    src/php/php_functions.c
    src/php/php_stdlib.c
    src/extensions/extension_manager.c
)

# Extension sources
set(EXTENSION_SOURCES
    src/extensions/curl/curl_polyfill.c
    src/extensions/mbstring/mbstring_polyfill.c
    src/extensions/json/json_polyfill.c
)

# Add all sources
set(ALL_SOURCES ${PHP2WASM_SOURCES} ${EXTENSION_SOURCES})

# Create the main executable
add_executable(php.wasm ${ALL_SOURCES})

# Link libraries
if(CMAKE_SYSTEM_NAME STREQUAL "WASI")
    target_link_libraries(php.wasm 
        wasi-emulated-process-clocks
        wasi-emulated-signal
    )
endif()

# Set output properties
set_target_properties(php.wasm PROPERTIES
    OUTPUT_NAME "php"
    SUFFIX ".wasm"
)

# Compiler-specific options
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(php.wasm PRIVATE
        -O3
        -flto
        -fvisibility=hidden
        -fno-stack-protector
        -fno-unwind-tables
        -fno-asynchronous-unwind-tables
    )
    target_link_options(php.wasm PRIVATE
        -Wl,--lto-O3
        -Wl,--strip-all
        -Wl,--gc-sections
        -Wl,--no-export-dynamic
    )
endif()

# Debug build options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(php.wasm PRIVATE DEBUG=1)
    target_compile_options(php.wasm PRIVATE -g -O0)
    target_link_options(php.wasm PRIVATE -Wl,--strip-debug)
endif()

# Install targets
install(TARGETS php.wasm DESTINATION bin)

# Custom targets
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/dist
)

# Test target
enable_testing()
add_custom_target(test
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS php.wasm
)

# Package target
set(CPACK_PACKAGE_NAME "php2wasm")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PHP to WebAssembly compiler")
set(CPACK_PACKAGE_VENDOR "Mehmet T. AKALIN")

include(CPack)
