<?php
/**
 * Worker PHP Entry Point
 * This file is executed by the WebAssembly module in the Cloudflare Worker
 */

// Set content type
header('Content-Type: text/html; charset=utf-8');

// Get request information
$method = $_SERVER['REQUEST_METHOD'] ?? 'GET';
$uri = $_SERVER['REQUEST_URI'] ?? '/';
$host = $_SERVER['HTTP_HOST'] ?? 'localhost';
$user_agent = $_SERVER['HTTP_USER_AGENT'] ?? 'php2wasm-worker';
$query_string = $_SERVER['QUERY_STRING'] ?? '';

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PHP2WASM Cloudflare Worker</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            max-width: 1000px;
            margin: 0 auto;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }
        .container {
            background: white;
            padding: 40px;
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
        }
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 30px;
            font-size: 2.5em;
        }
        .info-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin: 30px 0;
        }
        .info-card {
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
            border-left: 4px solid #007bff;
        }
        .info-card h3 {
            margin-top: 0;
            color: #007bff;
        }
        .status {
            background: #d4edda;
            color: #155724;
            padding: 15px;
            border-radius: 5px;
            text-align: center;
            margin: 20px 0;
            font-weight: bold;
        }
        .code-block {
            background: #f8f9fa;
            border: 1px solid #e9ecef;
            border-radius: 5px;
            padding: 15px;
            font-family: 'Courier New', monospace;
            overflow-x: auto;
            margin: 10px 0;
        }
        .footer {
            text-align: center;
            margin-top: 40px;
            padding-top: 20px;
            border-top: 1px solid #dee2e6;
            color: #6c757d;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 PHP2WASM Cloudflare Worker</h1>
        
        <div class="status">
            ✅ PHP is running successfully in WebAssembly on Cloudflare Workers!
        </div>

        <div class="info-grid">
            <div class="info-card">
                <h3>Request Information</h3>
                <p><strong>Method:</strong> <?php echo htmlspecialchars($method); ?></p>
                <p><strong>URI:</strong> <?php echo htmlspecialchars($uri); ?></p>
                <p><strong>Host:</strong> <?php echo htmlspecialchars($host); ?></p>
                <p><strong>Query String:</strong> <?php echo htmlspecialchars($query_string); ?></p>
            </div>

            <div class="info-card">
                <h3>Server Information</h3>
                <p><strong>PHP Version:</strong> <?php echo phpversion(); ?></p>
                <p><strong>User Agent:</strong> <?php echo htmlspecialchars($user_agent); ?></p>
                <p><strong>Current Time:</strong> <?php echo date('Y-m-d H:i:s T'); ?></p>
                <p><strong>Memory Usage:</strong> <?php echo number_format(memory_get_usage(true) / 1024 / 1024, 2); ?> MB</p>
            </div>

            <div class="info-card">
                <h3>Environment</h3>
                <p><strong>App Environment:</strong> <?php echo $_ENV['APP_ENV'] ?? 'not set'; ?></p>
                <p><strong>WASM Runtime:</strong> Cloudflare Workers</p>
                <p><strong>Platform:</strong> WebAssembly (WASI)</p>
                <p><strong>Architecture:</strong> wasm32</p>
            </div>

            <div class="info-card">
                <h3>Capabilities</h3>
                <p><strong>File I/O:</strong> <?php echo is_readable('.') ? '✅ Available' : '❌ Limited'; ?></p>
                <p><strong>Network:</strong> <?php echo function_exists('curl_init') ? '✅ Available' : '❌ Limited'; ?></p>
                <p><strong>Extensions:</strong> <?php echo count(get_loaded_extensions()); ?> loaded</p>
                <p><strong>Functions:</strong> <?php echo count(get_defined_functions()['user']); ?> user-defined</p>
            </div>
        </div>

        <h3>Available Extensions</h3>
        <div class="code-block">
<?php
$extensions = get_loaded_extensions();
sort($extensions);
echo implode(', ', $extensions);
?>
        </div>

        <h3>Sample PHP Code Execution</h3>
        <div class="code-block">
<?php
// Demonstrate some PHP functionality
$numbers = [1, 2, 3, 4, 5];
$squared = array_map(function($n) { return $n * $n; }, $numbers);
echo "Original: " . implode(', ', $numbers) . "\n";
echo "Squared:  " . implode(', ', $squared) . "\n";

// Simple calculation
$fibonacci = [0, 1];
for ($i = 2; $i < 10; $i++) {
    $fibonacci[$i] = $fibonacci[$i-1] + $fibonacci[$i-2];
}
echo "Fibonacci: " . implode(', ', $fibonacci) . "\n";
?>
        </div>

        <div class="footer">
            <p>Powered by <strong>PHP2WASM</strong> - PHP running in WebAssembly</p>
            <p>Generated at <?php echo date('Y-m-d H:i:s'); ?> | Memory: <?php echo number_format(memory_get_peak_usage(true) / 1024 / 1024, 2); ?> MB peak</p>
        </div>
    </div>
</body>
</html>
