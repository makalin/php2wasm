/**
 * Cloudflare Worker Example
 * Demonstrates how to use php2wasm in a Cloudflare Worker
 */

export default {
  async fetch(request, env, ctx) {
    try {
      // Load the PHP WebAssembly module
      const wasmModule = await WebAssembly.compile(
        await (await fetch('php.wasm')).arrayBuffer()
      );

      // Create WASI instance
      const wasi = new WASI({
        args: ['php.wasm', 'index.php'],
        env: {
          APP_ENV: 'production',
          REQUEST_METHOD: request.method,
          REQUEST_URI: request.url,
          HTTP_HOST: request.headers.get('host') || 'localhost',
          HTTP_USER_AGENT: request.headers.get('user-agent') || 'php2wasm-worker',
          QUERY_STRING: new URL(request.url).search.substring(1)
        },
        preopens: {
          '/': '/'
        }
      });

      // Instantiate the WebAssembly module
      const instance = await WebAssembly.instantiate(wasmModule, {
        ...wasi.getImportObject()
      });

      // Start the WASI instance
      wasi.start(instance);

      // Get output from stdout
      const output = wasi.stdoutString?.() || 'No output';

      // Return response
      return new Response(output, {
        headers: {
          'Content-Type': 'text/html; charset=utf-8',
          'X-Powered-By': 'php2wasm',
          'X-WASM-Runtime': 'Cloudflare Workers'
        }
      });

    } catch (error) {
      console.error('Worker error:', error);
      
      return new Response(`
        <!DOCTYPE html>
        <html>
        <head>
          <title>PHP2WASM Worker Error</title>
          <style>
            body { font-family: Arial, sans-serif; margin: 40px; }
            .error { background: #f8d7da; color: #721c24; padding: 20px; border-radius: 5px; }
          </style>
        </head>
        <body>
          <h1>PHP2WASM Worker Error</h1>
          <div class="error">
            <p><strong>Error:</strong> ${error.message}</p>
            <p>This worker requires WASI support in Cloudflare Workers.</p>
          </div>
        </body>
        </html>
      `, {
        status: 500,
        headers: {
          'Content-Type': 'text/html; charset=utf-8'
        }
      });
    }
  }
};
