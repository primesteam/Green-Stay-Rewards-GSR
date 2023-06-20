<!DOCTYPE html>
<html>
<head>
    <title>GSR Customers</title>
    <style>
        body {
            background-color: grey;
        }

        #table-container {
            width: 70%;
            margin: 0 auto;
        }

        table {
            border-collapse: collapse;
            width: 100%;
            border: 3px solid white;
        }

        th, td {
            border: 3px solid black;
            padding: 8px;
            color: black; /* Updated text color */
        }

        .pagination {
            display: flex;
            justify-content: center;
            margin-top: 20px;
        }

        .pagination a {
            color: black;
            padding: 8px 16px;
            text-decoration: none;
            transition: background-color .3s;
            border: 1px solid #ddd;
        }

        .pagination a.active {
            background-color: #4CAF50;
            color: white; /* Updated text color */
            border: 1px solid #4CAF50;
        }

        .pagination a:hover:not(.active) {
            background-color: #ddd;
        }

        th, td {
            font-size: medium;
            color: white; /* Updated text color */
        }
    </style>
</head>
<body>
<div id="table-container">
    <table id="myTable">
        <tr>
            <th>ID</th>
            <th>Firstname</th>
            <th>Lastname</th>
            <th>Points</th>
        </tr>
        <?php
        require 'private/Database.php';
        function generateData() {
            $data = [];
            $db = Database::connect();
            $stmt = $db->prepare("SELECT * FROM customer");
            $stmt->execute();
            $customers = $stmt->fetchAll();

            foreach ($customers as $cus) {
                $id = $cus['id'];
                $firstname = $cus['firstName'];
                $lastname = $cus['lastName'];
                $points = $cus['points'];
                $data[] = [$id, $firstname, $lastname, $points];
            }
            return $data;
        }

        $data = generateData();

        if (isset($_GET['page'])) {
            $pageNumber = $_GET['page'];
        } else {
            $pageNumber = 1;
        }

        $startIndex = ($pageNumber - 1) * 25;
        $endIndex = $startIndex + 25;

        for ($i = $startIndex; $i < $endIndex && $i < count($data); $i++) {
            echo "<tr>";
            echo "<td>" . $data[$i][0] . "</td>";
            echo "<td>" . $data[$i][1] . "</td>";
            echo "<td>" . $data[$i][2] . "</td>";
            echo "<td>" . $data[$i][3] . "</td>";
            echo "</tr>";
        }
        ?>
    </table>
</div>

<!-- Pagination -->
<div class="pagination">
    <?php
    $numPages = ceil(count($data) / 25);
    for ($i = 1; $i <= $numPages; $i++) {
        $activeClass = ($i == $pageNumber) ? "active" : "";
        echo '<a href="?page=' . $i . '" class="' . $activeClass . '">' . $i . '</a>';
    }
    ?>
</div>
</body>
</html>
