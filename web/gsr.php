<?php
require '../private/Database.php';

if (!isset($_GET)) return print "No data";
if (!isset($_GET['code'])) return print "No code";

$code = $_GET['code'];

$db = Database::connect();
$stmt = $db->prepare("SELECT * FROM gsr.room_sensor WHERE code=:code");
$stmt->bindParam(':code', $code);
$stmt->execute();
$sensors = $stmt->fetchAll();

// Check if the entry exists
if (count($sensors) !== 0) {
    $roomId = $sensors[0]['room'];

    $stmt = $db->prepare("SELECT * FROM gsr.customer WHERE room=:room");
    $stmt->bindParam(':room', $roomId);
    $stmt->execute();
    $customers = $stmt->fetchAll();

    $customerId = 0;
    foreach ($customers as $cus) {
        $comparisonDate = date('Y-m-d');
        $checkIn = $cus['checkIn'];
        $checkOut = $cus['checkOut'];

        if ($checkIn < $comparisonDate && $checkOut > $comparisonDate) $customerId = $cus['id'];
    }

    if ($customerId === 0) return;
    $titles = array_keys($_GET);

    foreach ($titles as $title) {
        $sen = 0;
        $error = false;


        if ($title == "objectTemp") $sen = 1;
        else if ($title == "roomTemp") $sen = 2;
        else if ($title == "waterTemp") $sen = 3;
        else if ($title == "waterFlow") $sen = 4;
        else $error = true;

        if (!$error) {
            $foundSensor = [];

            foreach ($sensors as $sensor) {
                if ($sensor['type'] == $sen) {
                    $foundSensor = $sensor;
                    break;
                }
            }

            if (isset($foundSensor['id'])) {
                $stmt = $db->prepare('INSERT INTO gsr.sensor_value (customer, sensor, value) VALUES (:cus, :sen, :val)');
                $stmt->bindParam(':cus', $customerId, PDO::PARAM_INT);
                $stmt->bindParam(':sen', $foundSensor['id'], PDO::PARAM_INT);
                $stmt->bindParam(':val', $_GET[$title]);
                $stmt->execute();
            } else {
                print "No sensor found";
            }
        }
    }
    print "Got Data!";
} else {
    return print "No sensor found";
}