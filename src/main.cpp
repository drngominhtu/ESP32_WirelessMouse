/*
Developed by: Dr.ngominhtu
Version: 1.1
Date: 2024-01-15

ESP32 Air Mouse with Anti-Drift Enhancement
This code implements an ESP32-based air mouse using the MPU6050 sensor and BLE Mouse library.
*/


#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BleMouse.h>

// MPU6050 object
Adafruit_MPU6050 mpu;

// BLE Mouse object with enhanced configuration
BleMouse bleMouse("ESP32-AirMouse", "ESP32-Dev", 100);

// Mouse sensitivity settings
float mousesensitivity = 3.0;
float scrollSensitivity = 1.0;

// Enhanced calibration and filtering
float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;
bool isCalibrated = false;
unsigned long lastUpdate = 0;
unsigned long lastConnectionCheck = 0;

// Anti-drift variables
float gyroIntegralX = 0, gyroIntegralY = 0; // Accumulate small movements
unsigned long lastDriftCorrection = 0;
float driftCorrectionX = 0, driftCorrectionY = 0;
bool enableDriftCorrection = true;

// Movement detection for auto-calibration
float movementThreshold = 0.02; // rad/s
unsigned long stillStartTime = 0;
bool isDeviceStill = false;
const unsigned long stillDuration = 3000; // 3 seconds

// Built-in LED for status indication
#define LED_PIN 2

// Function prototypes
void calibrateGyro();
void updateMousePosition();
void scanI2C();
void handleSerialCommands();
void detectStillness(float gyroX, float gyroY, float gyroZ);
void performDriftCorrection();

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== ESP32 Air Mouse v1.1 ===");
    Serial.println("Anti-Drift Enhanced Version");
    Serial.println("Initializing...");
    
    // Initialize LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Initialize I2C with specific pins
    Wire.begin(21, 22); // SDA=21, SCL=22 for ESP32 DevKit
    
    Serial.println("Starting MPU6050...");
    
    // Initialize MPU6050 with retry logic
    int retryCount = 0;
    while (!mpu.begin() && retryCount < 5) {
        Serial.printf("MPU6050 initialization attempt %d failed, retrying...\n", retryCount + 1);
        delay(1000);
        retryCount++;
    }
    
    if (retryCount >= 5) {
        Serial.println("Failed to initialize MPU6050 after 5 attempts!");
        Serial.println("Please check wiring:");
        Serial.println("  VCC -> 3.3V");
        Serial.println("  GND -> GND");
        Serial.println("  SDA -> GPIO 21");
        Serial.println("  SCL -> GPIO 22");
        Serial.println("Scanning I2C bus...");
        scanI2C();
        
        while (1) {
            digitalWrite(LED_PIN, HIGH);
            delay(200);
            digitalWrite(LED_PIN, LOW);
            delay(200);
        }
    }
    
    Serial.println("MPU6050 Found!");
    
    // Configure MPU6050 with optimal settings for stability
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ); // Lower bandwidth for less noise
    
    Serial.println("Calibrating gyroscope... Keep device VERY still for 5 seconds");
    
    // Enhanced calibration
    calibrateGyro();
    
    Serial.println("Starting BLE Mouse...");
    
    // Initialize BLE Mouse
    bleMouse.begin();
    
    Serial.println("BLE Mouse initialized");
    Serial.println("Device Name: ESP32-AirMouse");
    Serial.println("Anti-drift correction: ENABLED");
    Serial.println("Available commands: s=scan, r=restart BLE, c=calibrate, i=info, t=toggle drift correction");
    
    // LED indication for setup complete
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }
    
    Serial.println("=== Setup Complete ===");
    Serial.println("Waiting for BLE connection...");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Handle serial commands
    handleSerialCommands();
    
    // Check connection status every 2 seconds
    if (currentTime - lastConnectionCheck > 2000) {
        if (bleMouse.isConnected()) {
            static bool wasConnectedBefore = false;
            if (!wasConnectedBefore) {
                Serial.println("🎉 BLE Mouse Connected!");
                Serial.println("Anti-drift system active");
                Serial.println("Move the device to control cursor");
                Serial.println("Tap for left click");
                wasConnectedBefore = true;
                
                // LED on when connected
                digitalWrite(LED_PIN, HIGH);
            }
        } else {
            Serial.println("⏳ Waiting for BLE connection...");
            Serial.println("   Device Name: ESP32-AirMouse");
            
            // Blink LED when not connected
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        }
        lastConnectionCheck = currentTime;
    }
    
    // Mouse functionality only when connected
    if (bleMouse.isConnected()) {
        // Update mouse position every 20ms for smooth movement
        if (currentTime - lastUpdate > 20) {
            updateMousePosition();
            lastUpdate = currentTime;
        }
    }
    
    // Perform drift correction every 30 seconds
    if (enableDriftCorrection && (currentTime - lastDriftCorrection > 30000)) {
        performDriftCorrection();
        lastDriftCorrection = currentTime;
    }
    
    delay(5);
}

// Enhanced calibration with more samples
void calibrateGyro() {
    Serial.println("Enhanced calibration starting...");
    Serial.println("Keep device COMPLETELY STILL for 5 seconds...");
    
    delay(2000); // Wait 2 seconds before starting
    
    float sumX = 0, sumY = 0, sumZ = 0;
    int samples = 250; // More samples for better accuracy
    
    for (int i = 0; i < samples; i++) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        
        sumX += g.gyro.x;
        sumY += g.gyro.y;
        sumZ += g.gyro.z;
        
        delay(20); // 20ms * 250 = 5 seconds
        
        if (i % 50 == 0) {
            Serial.print(".");
        }
    }
    
    gyroOffsetX = sumX / samples;
    gyroOffsetY = sumY / samples;
    gyroOffsetZ = sumZ / samples;
    
    // Reset drift correction
    driftCorrectionX = 0;
    driftCorrectionY = 0;
    gyroIntegralX = 0;
    gyroIntegralY = 0;
    
    isCalibrated = true;
    
    Serial.println("\n✅ Enhanced calibration complete!");
    Serial.printf("Base Offsets - X: %+.4f, Y: %+.4f, Z: %+.4f\n", 
                  gyroOffsetX, gyroOffsetY, gyroOffsetZ);
}

void updateMousePosition() {
    if (!isCalibrated) return;
    
    // Get sensor data
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    // Apply calibration offsets AND drift correction
    float gyroX = g.gyro.x - gyroOffsetX - driftCorrectionX;
    float gyroY = g.gyro.y - gyroOffsetY - driftCorrectionY;
    float gyroZ = g.gyro.z - gyroOffsetZ;
    
    // Detect if device is still for auto-drift correction
    detectStillness(gyroX, gyroY, gyroZ);
    
    // Enhanced deadzone with adaptive threshold
    float baseDeadzone = 0.02; // Smaller base deadzone
    float adaptiveDeadzone = baseDeadzone;
    
    // Increase deadzone if device has been still recently
    if (isDeviceStill) {
        adaptiveDeadzone = baseDeadzone * 2.0;
    }
    
    // Apply deadzone
    if (abs(gyroX) < adaptiveDeadzone) gyroX = 0;
    if (abs(gyroY) < adaptiveDeadzone) gyroY = 0;
    if (abs(gyroZ) < adaptiveDeadzone) gyroZ = 0;
    
    // Accumulate small movements for drift detection
    if (enableDriftCorrection) {
        gyroIntegralX += gyroX * 0.02; // Integrate over time
        gyroIntegralY += gyroY * 0.02;
        
        // Limit integral windup
        gyroIntegralX = constrain(gyroIntegralX, -1.0, 1.0);
        gyroIntegralY = constrain(gyroIntegralY, -1.0, 1.0);
    }
    
    // Calculate mouse movement with improved mapping
    int deltaX = (int)(gyroY * mousesensitivity * 12);
    int deltaY = (int)(-gyroX * mousesensitivity * 12);
    
    // Limit maximum movement per update
    deltaX = constrain(deltaX, -25, 25);
    deltaY = constrain(deltaY, -25, 25);
    
    // Only send movement if above threshold
    if (abs(deltaX) > 0 || abs(deltaY) > 0) {
        bleMouse.move(deltaX, deltaY);
        
        // Debug output (uncomment for debugging)
        // Serial.printf("Move: X=%d, Y=%d | Gyro: X=%.4f, Y=%.4f | Drift: X=%.4f, Y=%.4f\n", 
        //               deltaX, deltaY, gyroX, gyroY, driftCorrectionX, driftCorrectionY);
    }
    
    // Handle scroll with Z-axis gyro
    int scroll = (int)(gyroZ * scrollSensitivity * 8);
    scroll = constrain(scroll, -5, 5);
    
    if (abs(scroll) > 1) {
        bleMouse.move(0, 0, scroll);
    }
    
    // Enhanced tap detection
    float accelMagnitude = sqrt(a.acceleration.x * a.acceleration.x + 
                               a.acceleration.y * a.acceleration.y + 
                               a.acceleration.z * a.acceleration.z);
    
    static float lastAccelMagnitude = 9.8;
    static unsigned long lastTapTime = 0;
    
    // Tap detection with better filtering
    float accelDiff = abs(accelMagnitude - lastAccelMagnitude);
    
    if (accelDiff > 4.0 && 
        (millis() - lastTapTime) > 400) {
        
        Serial.printf("👆 Tap detected (%.1fg) - Left Click\n", accelDiff);
        bleMouse.click(MOUSE_LEFT);
        lastTapTime = millis();
    }
    
    lastAccelMagnitude = accelMagnitude;
}

void detectStillness(float gyroX, float gyroY, float gyroZ) {
    float totalMovement = sqrt(gyroX*gyroX + gyroY*gyroY + gyroZ*gyroZ);
    
    if (totalMovement < movementThreshold) {
        if (!isDeviceStill) {
            stillStartTime = millis();
            isDeviceStill = true;
        }
    } else {
        isDeviceStill = false;
    }
}

void performDriftCorrection() {
    if (!enableDriftCorrection || !isCalibrated) return;
    
    // If device has been still for a while, update drift correction
    if (isDeviceStill && (millis() - stillStartTime > stillDuration)) {
        
        // Calculate drift correction based on accumulated integral
        float driftFactorX = gyroIntegralX * 0.1; // 10% correction
        float driftFactorY = gyroIntegralY * 0.1;
        
        // Apply drift correction gradually
        driftCorrectionX += driftFactorX;
        driftCorrectionY += driftFactorY;
        
        // Limit drift correction to reasonable values
        driftCorrectionX = constrain(driftCorrectionX, -0.1, 0.1);
        driftCorrectionY = constrain(driftCorrectionY, -0.1, 0.1);
        
        // Reset integral
        gyroIntegralX *= 0.5; // Decay integral
        gyroIntegralY *= 0.5;
        
        Serial.printf("🔧 Drift correction updated: X=%+.4f, Y=%+.4f\n", 
                     driftCorrectionX, driftCorrectionY);
    }
}

void scanI2C() {
    Serial.println("Scanning I2C devices...");
    byte count = 0;
    
    for (byte i = 8; i < 120; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) {
            Serial.printf("Found I2C device at address 0x%02X", i);
            if (i == 0x68) {
                Serial.print(" (MPU6050)");
            }
            Serial.println();
            count++;
        }
    }
    
    if (count == 0) {
        Serial.println("No I2C devices found!");
    } else {
        Serial.printf("Found %d I2C device(s)\n", count);
    }
}

void handleSerialCommands() {
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 's':
            case 'S':
                scanI2C();
                break;
                
            case 'r':
            case 'R':
                Serial.println("Restarting BLE...");
                bleMouse.end();
                delay(1000);
                bleMouse.begin();
                Serial.println("BLE restarted");
                break;
                
            case 'c':
            case 'C':
                Serial.println("Recalibrating gyroscope...");
                calibrateGyro();
                break;
                
            case 't':
            case 'T':
                enableDriftCorrection = !enableDriftCorrection;
                Serial.printf("Drift correction: %s\n", enableDriftCorrection ? "ENABLED" : "DISABLED");
                if (!enableDriftCorrection) {
                    driftCorrectionX = 0;
                    driftCorrectionY = 0;
                    gyroIntegralX = 0;
                    gyroIntegralY = 0;
                }
                break;
                
            case 'z':
            case 'Z':
                // Zero drift correction manually
                Serial.println("Zeroing drift corrections...");
                driftCorrectionX = 0;
                driftCorrectionY = 0;
                gyroIntegralX = 0;
                gyroIntegralY = 0;
                Serial.println("Drift corrections reset to zero");
                break;
                
            case '+':
                mousesensitivity = constrain(mousesensitivity + 0.5, 0.5, 10.0);
                Serial.printf("Mouse sensitivity: %.1f\n", mousesensitivity);
                break;
                
            case '-':
                mousesensitivity = constrain(mousesensitivity - 0.5, 0.5, 10.0);
                Serial.printf("Mouse sensitivity: %.1f\n", mousesensitivity);
                break;
                
            case 'i':
            case 'I':
                Serial.println("=== Device Information ===");
                Serial.printf("Device Name: ESP32-AirMouse\n");
                Serial.printf("BLE Connected: %s\n", bleMouse.isConnected() ? "Yes" : "No");
                Serial.printf("Calibrated: %s\n", isCalibrated ? "Yes" : "No");
                Serial.printf("Mouse Sensitivity: %.1f\n", mousesensitivity);
                Serial.printf("Scroll Sensitivity: %.1f\n", scrollSensitivity);
                Serial.printf("Drift Correction: %s\n", enableDriftCorrection ? "ENABLED" : "DISABLED");
                Serial.printf("Base Offsets: X=%+.4f, Y=%+.4f, Z=%+.4f\n", 
                             gyroOffsetX, gyroOffsetY, gyroOffsetZ);
                Serial.printf("Drift Corrections: X=%+.4f, Y=%+.4f\n", 
                             driftCorrectionX, driftCorrectionY);
                Serial.printf("Device Still: %s\n", isDeviceStill ? "Yes" : "No");
                break;
                
            case 'd':
            case 'D':
                // Toggle debug mode
                static bool debugMode = false;
                debugMode = !debugMode;
                Serial.printf("Debug mode: %s\n", debugMode ? "ON" : "OFF");
                break;
                
            case 'h':
            case 'H':
            case '?':
                Serial.println("=== Available Commands ===");
                Serial.println("s - Scan I2C devices");
                Serial.println("r - Restart BLE");
                Serial.println("c - Recalibrate gyroscope");
                Serial.println("t - Toggle drift correction");
                Serial.println("z - Zero drift corrections");
                Serial.println("+ - Increase mouse sensitivity");
                Serial.println("- - Decrease mouse sensitivity");
                Serial.println("i - Show device information");
                Serial.println("d - Toggle debug mode");
                Serial.println("h - Show this help");
                break;
                
            default:
                if (cmd != '\n' && cmd != '\r') {
                    Serial.printf("Unknown command: %c (type 'h' for help)\n", cmd);
                }
                break;
        }
    }
}
