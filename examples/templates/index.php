<?php
/**
 * Template Example
 * Demonstrates basic templating with embedded PHP
 */

// Template data
$title = "PHP2WASM Template Example";
$user = [
    'name' => 'John Doe',
    'email' => 'john@example.com',
    'role' => 'Developer'
];
$items = [
    ['name' => 'Apple', 'price' => 1.50, 'quantity' => 5],
    ['name' => 'Banana', 'price' => 0.75, 'quantity' => 10],
    ['name' => 'Orange', 'price' => 2.00, 'quantity' => 3]
];

// Calculate totals
$total = 0;
foreach ($items as $item) {
    $total += $item['price'] * $item['quantity'];
}

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title><?php echo htmlspecialchars($title); ?></title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            background: white;
            padding: 30px;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            border-bottom: 2px solid #007bff;
            padding-bottom: 10px;
        }
        .user-info {
            background: #e9ecef;
            padding: 15px;
            border-radius: 5px;
            margin: 20px 0;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
        }
        th, td {
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        th {
            background-color: #007bff;
            color: white;
        }
        .total {
            font-size: 1.2em;
            font-weight: bold;
            text-align: right;
            margin-top: 20px;
            padding: 10px;
            background: #28a745;
            color: white;
            border-radius: 5px;
        }
        .footer {
            margin-top: 30px;
            padding-top: 20px;
            border-top: 1px solid #ddd;
            text-align: center;
            color: #666;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1><?php echo htmlspecialchars($title); ?></h1>
        
        <div class="user-info">
            <h3>User Information</h3>
            <p><strong>Name:</strong> <?php echo htmlspecialchars($user['name']); ?></p>
            <p><strong>Email:</strong> <?php echo htmlspecialchars($user['email']); ?></p>
            <p><strong>Role:</strong> <?php echo htmlspecialchars($user['role']); ?></p>
        </div>

        <h3>Shopping Cart</h3>
        <table>
            <thead>
                <tr>
                    <th>Item</th>
                    <th>Price</th>
                    <th>Quantity</th>
                    <th>Total</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($items as $item): ?>
                <tr>
                    <td><?php echo htmlspecialchars($item['name']); ?></td>
                    <td>$<?php echo number_format($item['price'], 2); ?></td>
                    <td><?php echo $item['quantity']; ?></td>
                    <td>$<?php echo number_format($item['price'] * $item['quantity'], 2); ?></td>
                </tr>
                <?php endforeach; ?>
            </tbody>
        </table>

        <div class="total">
            Total: $<?php echo number_format($total, 2); ?>
        </div>

        <div class="footer">
            <p>Generated on <?php echo date('Y-m-d H:i:s'); ?> using PHP2WASM</p>
            <p>PHP Version: <?php echo phpversion(); ?></p>
        </div>
    </div>
</body>
</html>
