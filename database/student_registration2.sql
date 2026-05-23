CREATE DATABASE IF NOT EXISTS student_registration2;

USE student_registration2;

-- =========================
-- STUDENTS TABLE
-- =========================
CREATE TABLE students1 (
    id INT AUTO_INCREMENT PRIMARY KEY,
    uid VARCHAR(50) NOT NULL UNIQUE,
    Student_ID VARCHAR(50) NOT NULL,
    School_Program VARCHAR(50) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =========================
-- VOUCHER CODES
-- =========================
CREATE TABLE voucher_codes1 (
    id INT AUTO_INCREMENT PRIMARY KEY,
    code VARCHAR(100) NOT NULL UNIQUE,
    status ENUM('unreleased','released') DEFAULT 'unreleased',
    Used_By VARCHAR(50) DEFAULT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =========================
-- VOUCHER USAGE LOG
-- =========================
CREATE TABLE voucher_usage_log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    uid VARCHAR(50) NOT NULL,
    School_Program VARCHAR(50) NOT NULL,
    voucher_code VARCHAR(100) NOT NULL,
    usage_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =========================
-- SCHOOL PROGRAM ANALYTICS
-- =========================
CREATE TABLE schoolprogram_usage_log (
    School_Program VARCHAR(50) PRIMARY KEY,
    voucher_count INT DEFAULT 0
);

-- =========================
-- SAMPLE VOUCHERS
-- =========================
INSERT INTO voucher_codes1 (code, status) VALUES
('WIFI-ABC123', 'unreleased'),
('WIFI-XYZ789', 'unreleased'),
('WIFI-TEST456', 'unreleased');