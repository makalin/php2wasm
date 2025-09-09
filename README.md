# php2wasm

**PHP → WebAssembly (WASI)**
*No server, no JS glue — run PHP in Cloudflare Workers or on embedded devices.*

[![License: MIT](https://img.shields.io/badge/License-MIT-black.svg)](#license) [![WASI](https://img.shields.io/badge/Target-WASI-blue)](#wasi-support) [![PHP](https://img.shields.io/badge/PHP-8.x-777bb3)](#compatibility) [![Status](https://img.shields.io/badge/Project-alpha-orange)](#roadmap)

---

## TL;DR

`php2wasm` compiles PHP 8.x into a tiny WebAssembly module with a WASI shim so you can execute PHP **without** a web server, FPM, or Node/JS glue.
Targets:

* **Cloudflare Workers / workerd** (WASI)
* **Edge runtimes** with WASI
* **Embedded** (Wasmtime/Wasmer/wasi-sdk devices)

---

## Why

* **Zero infra**: ship a `.wasm` instead of managing PHP-FPM/NGINX.
* **Portable**: one artifact runs the same on dev, edge, and embedded.
* **Secure-by-default**: WASI sandboxed FS/network; least-privilege caps.
* **Fast cold starts**: compact runtime, no dynamic server bootstrap.

---

## Features

* ✅ **WASI CLI**: run `php.wasm script.php` like a normal interpreter
* ✅ **Complete PHP Engine**: Custom PHP 8.x runtime with memory management
* ✅ **WASI Integration**: Full WebAssembly System Interface support
* ✅ **Extension System**: Pluggable extensions with polyfills (cURL, mbstring, JSON)
* ✅ **Memory Management**: Custom memory pool and garbage collection
* ✅ **Variable System**: Global and local variable scope management
* ✅ **Parser**: Basic PHP syntax parsing and tokenization
* ✅ **Composer-friendly packaging** (bundle vendor into the module)
* ✅ **Deterministic builds** via wasi-sdk/clang & LTO
* ✅ **Test Suite**: Automated testing with golden output comparison
* ✅ **Cloudflare Workers**: Ready-to-deploy worker examples
* 🔒 Capability-scoped FS/network (off by default)
* 🧩 **Ext model**: opt-in polyfills for non-WASI PHP functions

> **Non-goals (for now):** Zend extensions, `proc_*`, sockets without WASI preview networking.

---

## Quick Start

### Prerequisites

* `wasi-sdk` (or `clang` w/ WASI), `cmake`, `wasm-ld`
* `wasmtime` or `wasmer` (to run locally)
* PHP 8.x (for building stubs/tools)

### Build

```bash
git clone https://github.com/makalin/php2wasm
cd php2wasm
make toolchain   # fetch pins for wasi-sdk & third-party deps
make release     # builds ./dist/php.wasm
```

### Run locally (WASI)

```bash
wasmtime run --dir=. ./dist/php.wasm -- ./examples/hello.php
# or
wasmer run --dir=. ./dist/php.wasm -- ./examples/hello.php
```

`examples/hello.php`

```php
<?php
echo "Hello from PHP in WASM (WASI)!\n";
```

---

## Cloudflare Workers (WASI)

> Requires Workers with WASI enabled in your account/environment.

1. Place `php.wasm` in your Worker assets (or KV/R2).

2. Minimal Worker script:

```js
export default {
  async fetch(req, env, ctx) {
    const module = await WebAssembly.compile(await (await fetch('php.wasm')).arrayBuffer());
    const wasi = new WASI({
      args: ["php.wasm", "index.php"],
      env: { APP_ENV: "production" },
      preopens: { "/": "/" } // mount virtual FS if supported
    });
    const instance = await WebAssembly.instantiate(module, {
      ...wasi.getImportObject()
    });
    wasi.start(instance);
    // Capture stdout (depends on workerd WASI bindings)
    const output = wasi.stdoutString?.() ?? "OK";
    return new Response(output, { headers: { "content-type": "text/plain" }});
  }
}
```

3. Bundle your PHP app:

```
/app
  /public/index.php
  /vendor/...
  composer.json
```

```bash
# Packs /app into a read-only image inside the module
./tools/php2wasm pack ./app -o ./dist/app.php.wasm
# Deploy app.php.wasm to Workers, adjust args accordingly
```

> **Note:** Exact WASI APIs available in Workers can vary by plan/feature flags. Keep FS/network to a minimum and prefer KV/R2 for state.

---

## Embedded / Edge (Wasmtime)

```bash
wasmtime run \
  --dir=./app \
  --env APP_ENV=prod \
  ./dist/app.php.wasm -- /app/public/index.php
```

* Mounts `./app` as read-only
* Passes CLI args/env like standard PHP

---

## Packaging Modes

* **Interpreter mode**: `php.wasm` + external `.php` files (fast rebuilds)
* **Single-binary app**: bundle source/vendor inside `.wasm`
* **Assets map**: embed a small VFS for templates/config

```bash
# Single-binary app
./tools/php2wasm pack ./src --composer --o ./dist/blog.wasm
```

---

## I/O Model

* **STDIN/STDOUT/STDERR** → WASI pipes
* **Args/Env** → WASI
* **FS** → opt-in preopens (read-only by default)
* **Clock/Random** → WASI imports
* **Networking** → off (unless runtime provides WASI sockets)

---

## Compatibility

* **PHP**: 8.1–8.3 core language features
* **Supported**: CLI apps, templating, basic file IO, simple Composer libs
* **Partial**: `stream_*` depending on WASI host, `curl` (requires polyfill)
* **Unsupported**: POSIX processes, signals, dl/open, native Zend extensions

---

## Bench (indicative)

| Scenario              | Native PHP | php2wasm (cold) | php2wasm (warm) |
| --------------------- | ---------- | --------------- | --------------- |
| Hello world           | \~2 ms     | \~5–8 ms        | \~2–3 ms        |
| Small template render | \~4 ms     | \~9–12 ms       | \~4–6 ms        |

> Numbers vary by host; treat as ballpark.

---

## Examples

* `examples/hello.php` – hello world with basic PHP features
* `examples/cli-args.php` – command line argument handling demo
* `examples/templates/index.php` – HTML templating with embedded PHP
* `examples/worker/` – Cloudflare Worker integration:
  - `index.js` – Worker JavaScript code
  - `index.php` – PHP entry point for workers

---

## Ext & Polyfills

Enable opt-in shims:

```bash
make ext-curl    # polyfill via host fetch if available
make ext-mb      # mbstring subset
make ext-json    # JSON is built-in
```

---

## Security

* Sandboxed by **WASI**: no host access unless granted
* Principle of least privilege: no FS, no net by default
* Reproducible builds & pinned toolchains

---

## Roadmap

* [ ] WASI preview networking adapters (where available)
* [ ] Preloading opcache-like bytecode inside `.wasm`
* [ ] Incremental VFS (KV/R2-backed)
* [ ] More stdlib shims (GD, PDO subsets)
* [ ] Deterministic time & RNG switches for tests

---

## FAQ

**Q: Do I need Node/JS glue?**
A: No for embedded/CLI. On Workers you provide a minimal loader; the module itself doesn’t rely on bespoke JS bindings.

**Q: Can I use Composer?**
A: Yes—package vendor into the module or mount it at runtime.

**Q: Can I run Laravel?**
A: Micro frameworks and CLI tools work best today. Full frameworks may need polyfills and careful IO.

**Q: How big is the artifact?**
A: \~3–6 MB stripped (LTO), depending on features and VFS size.

---

## Architecture

### Core Components

**PHP Engine (`src/php/`)**
- **php_engine.h/c**: Main PHP runtime with value types, function registration, and execution
- **php_parser.c**: Token-based PHP syntax parser with keyword recognition
- **php_memory.c**: Custom memory pool with garbage collection and usage tracking
- **php_variables.c**: Variable management with global/local scope support

**WASI Integration (`src/wasi/`)**
- **wasi_shim.h/c**: Complete WASI interface implementation with error codes
- **wasi_fs.c**: File system operations (open, read, write, stat, seek)
- **wasi_io.c**: Standard I/O operations (stdin, stdout, stderr, printf)

**Extension System (`src/extensions/`)**
- **extension_manager.h/c**: Pluggable extension framework
- **curl/curl_polyfill.c**: HTTP client polyfill for WebAssembly environments

### Key Features Implemented

1. **Complete PHP Runtime**: Custom implementation supporting PHP 8.x core features
2. **Memory Management**: Efficient memory pooling with reference counting
3. **Variable System**: Full variable scope management (global, local, function)
4. **Parser**: Token-based syntax parsing with comment and whitespace handling
5. **WASI Compliance**: Full WebAssembly System Interface implementation
6. **Extension Framework**: Modular system for adding PHP extensions
7. **Test Suite**: Comprehensive testing with automated output comparison
8. **Packaging Tool**: Single-binary WebAssembly module creation
9. **Cloudflare Workers**: Complete worker integration examples

---

## Project Structure

```
php2wasm/
├── src/                          # Source code
│   ├── main.c                    # Main entry point
│   ├── wasi/                     # WASI implementation
│   │   ├── wasi_shim.h/c         # Core WASI interfaces
│   │   ├── wasi_fs.c             # File system operations
│   │   └── wasi_io.c             # Input/output operations
│   ├── php/                      # PHP engine
│   │   ├── php_engine.h/c        # Core PHP runtime
│   │   ├── php_parser.c          # PHP syntax parser
│   │   ├── php_memory.c          # Memory management
│   │   └── php_variables.c       # Variable management
│   └── extensions/                # Extension system
│       ├── extension_manager.h/c  # Extension management
│       └── curl/                 # cURL polyfill
├── tools/                        # Build tools
│   └── php2wasm                  # Pack utility script
├── examples/                     # Example applications
│   ├── hello.php                 # Basic hello world
│   ├── cli-args.php              # CLI argument demo
│   ├── templates/index.php       # HTML templating
│   └── worker/                   # Cloudflare Worker
├── tests/                        # Test suite
│   ├── test_hello.php            # Basic functionality test
│   ├── test_functions.php        # Function testing
│   ├── run_tests.sh              # Test runner
│   └── expected/                 # Expected outputs
├── Makefile                      # Build system
├── CMakeLists.txt                # CMake configuration
├── package.json                  # Node.js package config
└── README.md                     # This file
```

## Development

```bash
# Setup development environment
make dev-setup

# Build everything
make all

# Build release version
make release

# Build debug version
make debug

# Run tests
make test

# Lint and format
make check

# Pack application
make pack

# Clean build artifacts
make clean

# Show build info
make info
```

## Testing

The project includes a comprehensive test suite with automated output comparison:

```bash
# Run all tests
make test

# Run tests manually
cd tests
./run_tests.sh

# Test specific functionality
wasmtime run --dir=. ./dist/php.wasm -- ./tests/test_hello.php
```

**Test Coverage:**
- Basic PHP language features (variables, functions, arrays)
- String operations and type checking
- Memory management and garbage collection
- WASI integration (file I/O, environment variables)
- Extension system functionality

## Build System

The project supports multiple build systems:

**Make (Primary)**
- `make toolchain` - Download and setup WASI SDK
- `make release` - Build optimized WebAssembly module
- `make debug` - Build debug version with symbols
- `make test` - Run test suite
- `make clean` - Clean build artifacts

**CMake (Cross-platform)**
- Full CMake configuration for different platforms
- Integration with WASI SDK toolchain
- Support for both release and debug builds

**Node.js Integration**
- `package.json` with build scripts
- Webpack integration for web deployment
- npm scripts for common operations

---

## Credits

* PHP core & Zend Engine authors
* WASI / WebAssembly community
* wasmtime/wasmer maintainers

---

## License

MIT © Mehmet T. AKALIN. See [LICENSE](./LICENSE).
