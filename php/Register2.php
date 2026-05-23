<?php

// ================= DATABASE =================
$servername = "localhost";
$username   = "root";
$password   = "";
$dbname     = "student_registration2";

// ================= CONNECT =================
$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// ================= TIMEZONE =================
date_default_timezone_set('Asia/Manila');

// ================= POST REQUEST ONLY =================
if ($_SERVER["REQUEST_METHOD"] == "POST") {

    $uid             = $conn->real_escape_string($_POST['uid']);
    $studentNumber   = $conn->real_escape_string($_POST['studentNumber']);
    $schoolProgram   = $conn->real_escape_string($_POST['schoolProgram']);

    // ================= CHECK EXISTING UID =================
    $checkUidQuery =
        "SELECT * FROM students1 WHERE uid='$uid'";

    $result = $conn->query($checkUidQuery);

    if ($result->num_rows > 0) {

        echo "You are already registered.";

    } else {

        // ================= INSERT STUDENT =================
        $insertQuery = "
            INSERT INTO students1
            (
                uid,
                Student_ID,
                School_Program
            )
            VALUES
            (
                '$uid',
                '$studentNumber',
                '$schoolProgram'
            )
        ";

        if ($conn->query($insertQuery) === TRUE) {

            echo "Registration successful for student number $studentNumber.";

        } else {

            echo "Error inserting record: " . $conn->error;
        }
    }

} else {

    echo "Invalid request method.";
}

// ================= CLOSE CONNECTION =================
$conn->close();

?>