<?php
/**
 * Function Tests
 * Test various PHP functions and language features
 */

echo "=== PHP Function Tests ===\n\n";

// String functions
echo "String Functions:\n";
$str = "  Hello, World!  ";
echo "Original: '$str'\n";
echo "Trimmed: '" . trim($str) . "'\n";
echo "Length: " . strlen($str) . "\n";
echo "Uppercase: " . strtoupper($str) . "\n";
echo "Lowercase: " . strtolower($str) . "\n";
echo "Substring: " . substr($str, 2, 5) . "\n";
echo "Position of 'World': " . strpos($str, 'World') . "\n\n";

// Array functions
echo "Array Functions:\n";
$arr = [3, 1, 4, 1, 5, 9, 2, 6];
echo "Original array: " . implode(', ', $arr) . "\n";
echo "Count: " . count($arr) . "\n";
echo "Sum: " . array_sum($arr) . "\n";
echo "Max: " . max($arr) . "\n";
echo "Min: " . min($arr) . "\n";

sort($arr);
echo "Sorted: " . implode(', ', $arr) . "\n";

$unique = array_unique($arr);
echo "Unique: " . implode(', ', $unique) . "\n\n";

// Type checking functions
echo "Type Checking:\n";
$values = [
    42,
    3.14,
    "hello",
    true,
    null,
    [1, 2, 3]
];

foreach ($values as $value) {
    echo "Value: ";
    var_dump($value);
    echo "Type: " . gettype($value) . "\n";
    echo "Is array: " . (is_array($value) ? 'yes' : 'no') . "\n";
    echo "Is string: " . (is_string($value) ? 'yes' : 'no') . "\n";
    echo "Is numeric: " . (is_numeric($value) ? 'yes' : 'no') . "\n";
    echo "Is empty: " . (empty($value) ? 'yes' : 'no') . "\n";
    echo "Is set: " . (isset($value) ? 'yes' : 'no') . "\n\n";
}

// Math functions
echo "Math Functions:\n";
$num = 16;
echo "Number: $num\n";
echo "Square root: " . sqrt($num) . "\n";
echo "Power of 2: " . pow($num, 2) . "\n";
echo "Absolute value of -$num: " . abs(-$num) . "\n";
echo "Round 3.7: " . round(3.7) . "\n";
echo "Ceil 3.2: " . ceil(3.2) . "\n";
echo "Floor 3.8: " . floor(3.8) . "\n\n";

// Date functions
echo "Date Functions:\n";
echo "Current timestamp: " . time() . "\n";
echo "Current date: " . date('Y-m-d H:i:s') . "\n";
echo "Formatted date: " . date('l, F j, Y') . "\n";
echo "Day of year: " . date('z') . "\n\n";

// Variable functions
echo "Variable Functions:\n";
$var_name = 'test_var';
$$var_name = 'Hello from variable variable!';
echo "Variable variable: " . $test_var . "\n";

// Function definition and call
function greet($name = 'World') {
    return "Hello, $name!";
}

echo "Function call: " . greet() . "\n";
echo "Function call with arg: " . greet('PHP2WASM') . "\n\n";

echo "=== All tests completed ===\n";
?>
