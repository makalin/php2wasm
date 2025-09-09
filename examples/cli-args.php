<?php
/**
 * CLI Arguments Example
 * Demonstrates command line argument handling and environment variables
 */

echo "=== PHP CLI Arguments Example ===\n\n";

// Display command line arguments
echo "Command line arguments:\n";
echo "Script name: " . $argv[0] . "\n";
echo "Number of arguments: " . ($argc - 1) . "\n";

if ($argc > 1) {
    echo "Arguments:\n";
    for ($i = 1; $i < $argc; $i++) {
        echo "  [$i] " . $argv[$i] . "\n";
    }
} else {
    echo "No additional arguments provided.\n";
}

echo "\n";

// Display environment variables
echo "Environment variables:\n";
$env_vars = [
    'PATH',
    'HOME',
    'USER',
    'SHELL',
    'PWD',
    'LANG',
    'LC_ALL',
    'TZ'
];

foreach ($env_vars as $var) {
    $value = getenv($var);
    if ($value !== false) {
        echo "  $var = $value\n";
    }
}

echo "\n";

// Demonstrate argument parsing
if ($argc > 1) {
    echo "Argument parsing demonstration:\n";
    
    $options = [];
    $files = [];
    
    for ($i = 1; $i < $argc; $i++) {
        $arg = $argv[$i];
        
        if (strpos($arg, '--') === 0) {
            // Long option
            $option = substr($arg, 2);
            if (strpos($option, '=') !== false) {
                list($key, $value) = explode('=', $option, 2);
                $options[$key] = $value;
            } else {
                $options[$option] = true;
            }
        } elseif (strpos($arg, '-') === 0) {
            // Short option
            $option = substr($arg, 1);
            $options[$option] = true;
        } else {
            // File or positional argument
            $files[] = $arg;
        }
    }
    
    if (!empty($options)) {
        echo "  Options found:\n";
        foreach ($options as $key => $value) {
            if (is_bool($value)) {
                echo "    --$key\n";
            } else {
                echo "    --$key=$value\n";
            }
        }
    }
    
    if (!empty($files)) {
        echo "  Files/arguments:\n";
        foreach ($files as $file) {
            echo "    $file\n";
        }
    }
}

echo "\n";

// Demonstrate getopt-like functionality
echo "getopt() simulation:\n";
$shortopts = "hvV";
$longopts = ["help", "version", "verbose", "output:"];

// Simple getopt implementation
$parsed_options = [];
$remaining_args = [];

for ($i = 1; $i < $argc; $i++) {
    $arg = $argv[$i];
    
    if ($arg === '--help' || $arg === '-h') {
        $parsed_options['help'] = true;
    } elseif ($arg === '--version' || $arg === '-v') {
        $parsed_options['version'] = true;
    } elseif ($arg === '--verbose' || $arg === '-V') {
        $parsed_options['verbose'] = true;
    } elseif (strpos($arg, '--output=') === 0) {
        $parsed_options['output'] = substr($arg, 9);
    } elseif ($arg === '--output' && $i + 1 < $argc) {
        $parsed_options['output'] = $argv[++$i];
    } else {
        $remaining_args[] = $arg;
    }
}

if (!empty($parsed_options)) {
    echo "  Parsed options:\n";
    foreach ($parsed_options as $key => $value) {
        echo "    $key: $value\n";
    }
}

if (!empty($remaining_args)) {
    echo "  Remaining arguments:\n";
    foreach ($remaining_args as $arg) {
        echo "    $arg\n";
    }
}

echo "\n=== CLI Arguments Example Complete ===\n";
?>
