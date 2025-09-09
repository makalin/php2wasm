<?php
/**
 * Hello World Test
 * Basic functionality test
 */

echo "Hello from PHP2WASM test!\n";
echo "PHP Version: " . phpversion() . "\n";

// Test basic arithmetic
$result = 2 + 2;
echo "2 + 2 = $result\n";

// Test string operations
$greeting = "Hello";
$name = "World";
$message = "$greeting, $name!";
echo "$message\n";

// Test array operations
$numbers = [1, 2, 3, 4, 5];
$sum = array_sum($numbers);
echo "Sum of numbers: $sum\n";

// Test function
function factorial($n) {
    if ($n <= 1) return 1;
    return $n * factorial($n - 1);
}

echo "Factorial of 5: " . factorial(5) . "\n";

echo "Test completed successfully!\n";
?>
