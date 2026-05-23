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

// ================= POST REQUEST ONLY =================
if ($_SERVER["REQUEST_METHOD"] == "POST") {

    $uid = $conn->real_escape_string($_POST['uid']);

    // ================= CHECK REGISTERED UID =================
    $checkStudentQuery =
        "SELECT * FROM students1 WHERE uid='$uid'";

    $studentResult = $conn->query($checkStudentQuery);

    if ($studentResult->num_rows > 0) {

        $studentRow = $studentResult->fetch_assoc();

        $schoolProgram = $studentRow['School_Program'];

        // ================= CHECK VOUCHER LIMIT =================
        $checkUsageQuery = "
            SELECT
                COUNT(*) AS voucher_count,
                MAX(usage_time) AS last_usage_time
            FROM voucher_usage_log
            WHERE uid='$uid'
            AND usage_time >= NOW() - INTERVAL 1 HOUR
        ";

        $usageResult = $conn->query($checkUsageQuery);
        $usageRow = $usageResult->fetch_assoc();

        $voucherCount = (int)$usageRow['voucher_count'];
        $lastUsageTime = $usageRow['last_usage_time'];

        // ================= LIMIT REACHED =================
        if ($voucherCount >= 3) {

            echo "You have reached the maximum voucher usage of 3 codes per hour. Last used at: $lastUsageTime.";

        } else {

            // ================= GET AVAILABLE VOUCHER =================
            $fetchVoucherQuery = "
                SELECT *
                FROM voucher_codes1
                WHERE status='unreleased'
                LIMIT 1
            ";

            $voucherResult = $conn->query($fetchVoucherQuery);

            if ($voucherResult->num_rows > 0) {

                $voucherRow = $voucherResult->fetch_assoc();

                $code = $voucherRow['code'];

                // ================= UPDATE VOUCHER STATUS =================
                $updateVoucherQuery = "
                    UPDATE voucher_codes1
                    SET status='released',
                        Used_By='$uid'
                    WHERE code='$code'
                ";

                if ($conn->query($updateVoucherQuery) === TRUE) {

                    // ================= LOG USAGE =================
                    $logUsageQuery = "
                        INSERT INTO voucher_usage_log
                        (
                            uid,
                            School_Program,
                            voucher_code,
                            usage_time
                        )
                        VALUES
                        (
                            '$uid',
                            '$schoolProgram',
                            '$code',
                            NOW()
                        )
                    ";

                    $conn->query($logUsageQuery);

                    // ================= TRACK PROGRAM USAGE =================
                    $updateProgramUsageQuery = "
                        INSERT INTO schoolprogram_usage_log
                        (
                            School_Program,
                            voucher_count
                        )
                        VALUES
                        (
                            '$schoolProgram',
                            1
                        )
                        ON DUPLICATE KEY UPDATE
                            voucher_count = voucher_count + 1
                    ";

                    $conn->query($updateProgramUsageQuery);

                    $voucherCount++;

                    echo "Voucher Code: $code. You have used $voucherCount voucher(s) in the last hour.";

                } else {

                    echo "Error updating voucher status: " . $conn->error;
                }

            } else {

                echo "No available voucher codes at this time.";
            }
        }

    } else {

        echo "You are not registered.";
    }

} else {

    echo "Invalid request method.";
}

// ================= CLOSE CONNECTION =================
$conn->close();

?>