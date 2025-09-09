<?php
/**
 * Hello World Example
 * Basic PHP script demonstrating simple output
 */

echo "Hello from PHP in WASM (WASI)!\n";
echo "PHP Version: " . phpversion() . "\n";
echo "Current time: " . date('Y-m-d H:i:s') . "\n";

// Simple variable demonstration
$name = "World";
$greeting = "Hello, $name!";
echo $greeting . "\n";

// Basic arithmetic
$a = 10;
$b = 20;
$sum = $a + $b;
echo "Sum of $a and $b is: $sum\n";

// Array demonstration
$fruits = ['apple', 'banana', 'orange'];
echo "Fruits: " . implode(', ', $fruits) . "\n";

// Loop demonstration
echo "Counting from 1 to 5:\n";
for ($i = 1; $i <= 5; $i++) {
    echo "  $i\n";
}

// Function demonstration
function fibonacci($n) {
    if ($n <= 1) return $n;
    return fibonacci($n - 1) + fibonacci($n - 2);
}

echo "Fibonacci sequence (first 8 numbers):\n";
for ($i = 0; $i < 8; $i++) {
    echo "  F($i) = " . fibonacci($i) . "\n";
}

echo "\nHello World example completed successfully!\n";
?>
