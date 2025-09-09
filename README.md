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
* ✅ **`include`, `require`**, basic stdlib (file IO, env, args)
* ✅ **Composer-friendly packaging** (bundle vendor into the module)
* ✅ **Deterministic builds** via wasi-sdk/clang & LTO
* 🔒 Capability-scoped FS/network (off by default)
* 🧩 **Ext model**: opt-in polyfills for non-WASI PHP functions
* 🧪 Minimal test suite with golden outputs

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

* `examples/hello.php` – hello world
* `examples/cli-args.php` – argv/env demo
* `examples/templates` – tiny templating with bundled assets
* `examples/worker` – Worker bootstrap

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

## Development

```bash
# lint, test, format
make check
# run tests under wasmtime
make test
# debug with wasi-nn/wasmtime flags
make run DEBUG=1
```

---

## Credits

* PHP core & Zend Engine authors
* WASI / WebAssembly community
* wasmtime/wasmer maintainers

---

## License

MIT © Mehmet T. AKALIN. See [LICENSE](./LICENSE).
