# 🖱️ ESP32 Air Mouse with Anti-Drift Enhancement

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Language: C++](https://img.shields.io/badge/Language-C++-red.svg)](https://isocpp.org/)
[![Version: 1.1](https://img.shields.io/badge/Version-1.1-green.svg)](https://github.com/yourusername/esp32-air-mouse)

> **Transform your ESP32 into a wireless air mouse with advanced anti-drift technology**

## 📋 Table of Contents

- [🌟 Features](#-features)
- [📦 Hardware Requirements](#-hardware-requirements)
- [🔧 Installation & Setup](#-installation--setup)
- [🚀 Getting Started](#-getting-started)
- [🎮 Usage](#-usage)
- [⚙️ Configuration](#️-configuration)
- [🔧 Troubleshooting](#-troubleshooting)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)
- [👨‍💻 Author](#-author)

## 🌟 Features

### ✨ Core Functionality
- **🖱️ Wireless Air Mouse** - Control your computer cursor by moving the ESP32 device
- **📱 Bluetooth Low Energy (BLE)** - Connect wirelessly to any BLE-compatible device
- **👆 Tap-to-Click** - Tap the device for left mouse clicks
- **🔄 Scroll Control** - Rotate the device for scrolling
- **🎯 High Precision** - 50Hz update rate for smooth cursor movement

### 🧠 Advanced Anti-Drift Technology
- **🔧 Automatic Drift Correction** - Eliminates gyroscope drift over time
- **📊 Adaptive Deadzone** - Dynamic sensitivity adjustment based on device state
- **🎯 Enhanced Calibration** - 250-sample calibration for maximum accuracy
- **⏱️ Real-time Monitoring** - Continuous drift detection and correction

### 🛠️ Developer Features
- **📡 I2C Device Scanner** - Automatic hardware detection
- **💻 Serial Commands** - Real-time configuration and debugging
- **📊 Status Monitoring** - Connection statistics and performance metrics
- **🔄 Auto-Recovery** - Automatic BLE reconnection on disconnects

## 📦 Hardware Requirements

### 🔌 Essential Components
| Component | Specification | Quantity |
|-----------|---------------|----------|
| **ESP32 DevKit v1** | 240MHz dual-core, Bluetooth/WiFi | 1 |
| **MPU6050** | 6-axis gyroscope/accelerometer | 1 |
| **Jumper Wires** | Male-to-Female | 4 |
| **USB Cable** | Micro-USB for ESP32 | 1 |

### 🔗 Wiring Diagram

```
ESP32 DevKit v1    │  MPU6050
───────────────────┼──────────
3.3V               │  VCC
GND                │  GND
GPIO 21 (SDA)      │  SDA
GPIO 22 (SCL)      │  SCL
```

### 💡 Optional Components
- **🔋 Battery Pack** - For portable operation
- **📦 Enclosure** - 3D printed case for protection
- **🔘 Physical Buttons** - Additional input options

## 🔧 Installation & Setup

### 📋 Prerequisites
- **PlatformIO IDE** (recommended) or **Arduino IDE**
- **ESP32 Board Package**
- **Git** for cloning the repository

### 1️⃣ Clone Repository
```bash
git clone https://github.com/yourusername/esp32-air-mouse.git
cd esp32-air-mouse
```

### 2️⃣ Install Dependencies
Create or update `platformio.ini`:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200

lib_deps =
    adafruit/Adafruit MPU6050@^2.2.6
    adafruit/Adafruit Unified Sensor@^1.1.9
    t-vk/ESP32 BLE Mouse@^0.3.1
```

### 3️⃣ Hardware Setup
1. **Connect MPU6050** to ESP32 according to wiring diagram
2. **Power on ESP32** via USB
3. **Verify connections** using I2C scanner

### 4️⃣ Upload Code
```bash
# Using PlatformIO
pio run --target upload --target monitor

# Using Arduino IDE
# Open main.cpp, select ESP32 board, and upload
```

## 🚀 Getting Started

### 📡 First Time Setup

1. **📤 Upload the code** to your ESP32
2. **🔍 Open Serial Monitor** (115200 baud)
3. **📍 Follow calibration instructions** - keep device still for 5 seconds
4. **🔗 Pair with your device** - search for "ESP32-AirMouse" in Bluetooth settings

### 🖥️ Expected Serial Output
```
=== ESP32 Air Mouse v1.1 ===
Anti-Drift Enhanced Version
Initializing...
Starting MPU6050...
MPU6050 Found!
Enhanced calibration starting...
Keep device COMPLETELY STILL for 5 seconds...
.....
✅ Enhanced calibration complete!
Base Offsets - X: +0.0012, Y: -0.0034, Z: +0.0008
BLE Mouse initialized
Device Name: ESP32-AirMouse
Anti-drift correction: ENABLED
=== Setup Complete ===
🎉 BLE Mouse Connected!
```

## 🎮 Usage

### 🖱️ Mouse Controls
| Action | Gesture | Description |
|--------|---------|-------------|
| **Move Cursor** | Tilt device | Move cursor in corresponding direction |
| **Left Click** | Tap device | Quick tap for left mouse click |
| **Scroll** | Rotate device | Rotate around Z-axis to scroll |
| **Recalibrate** | Type `c` in Serial | Reset gyroscope calibration |

### 📊 Control Sensitivity
- **Default Mouse Sensitivity**: 3.0
- **Default Scroll Sensitivity**: 1.0
- **Adjustable Range**: 0.5 - 10.0

### 💡 Best Practices
- **🎯 Calibrate in stable position** - Place on flat surface during calibration
- **🔄 Recalibrate if drifting** - Use `c` command if cursor drifts
- **🔋 Monitor connection** - LED indicates connection status
- **⚡ Keep device charged** - Low battery affects performance

## ⚙️ Configuration

### 📡 Serial Commands

| Command | Function | Description |
|---------|----------|-------------|
| `h` | Help | Show all available commands |
| `i` | Info | Display device information and status |
| `c` | Calibrate | Recalibrate gyroscope |
| `s` | Scan | Scan for I2C devices |
| `r` | Restart | Restart BLE connection |
| `t` | Toggle | Enable/disable drift correction |
| `z` | Zero | Reset drift corrections to zero |
| `+` | Increase | Increase mouse sensitivity |
| `-` | Decrease | Decrease mouse sensitivity |
| `d` | Debug | Toggle debug mode |

### 🔧 Advanced Configuration

#### Sensitivity Adjustment
```cpp
// In main.cpp, modify these values:
float mousesensitivity = 3.0;    // Mouse movement sensitivity
float scrollSensitivity = 1.0;   // Scroll sensitivity
```

#### Anti-Drift Settings
```cpp
// Drift correction parameters:
float movementThreshold = 0.02;        // Stillness detection threshold
const unsigned long stillDuration = 3000;  // Time before drift correction
bool enableDriftCorrection = true;     // Enable/disable drift correction
```

#### BLE Configuration
```cpp
// BLE device settings:
BleMouse bleMouse("ESP32-AirMouse", "ESP32-Dev", 100);
```

## 🔧 Troubleshooting

### ❌ Common Issues

#### 🔗 Connection Problems
**Problem**: Device not appearing in Bluetooth list
```bash
# Solutions:
1. Type 'r' to restart BLE
2. Check if Bluetooth is enabled on target device
3. Clear Bluetooth cache (Android)
4. Restart ESP32
```

#### 📍 Drift Issues
**Problem**: Cursor drifts without movement
```bash
# Solutions:
1. Type 'c' to recalibrate
2. Type 'z' to zero drift corrections
3. Ensure device is on stable surface during calibration
4. Type 't' to toggle drift correction
```

#### 🔌 Hardware Issues
**Problem**: MPU6050 not detected
```bash
# Solutions:
1. Type 's' to scan I2C devices
2. Check wiring connections
3. Verify 3.3V power supply
4. Try different I2C address (0x68 or 0x69)
```

#### ⚡ Performance Issues
**Problem**: Laggy or unresponsive movement
```bash
# Solutions:
1. Check battery level
2. Reduce sensitivity with '-' command
3. Move closer to paired device
4. Restart BLE with 'r' command
```

### 📊 Debug Information

Use the `i` command to get detailed status:
```
=== Device Information ===
Device Name: ESP32-AirMouse
BLE Connected: Yes
Calibrated: Yes
Mouse Sensitivity: 3.0
Drift Correction: ENABLED
Base Offsets: X=+0.0012, Y=-0.0034, Z=+0.0008
Drift Corrections: X=+0.0001, Y=-0.0002
Device Still: No
```

## 🔧 Technical Specifications

### 📊 Performance Metrics
- **Update Rate**: 50Hz (20ms intervals)
- **BLE Latency**: 10-20ms typical
- **Movement Resolution**: ±25 pixels per update
- **Calibration Samples**: 250 samples over 5 seconds
- **Drift Correction**: Every 30 seconds when enabled

### 🔋 Power Consumption
- **Active Mode**: ~80mA
- **Connected Idle**: ~20mA
- **Advertising Mode**: ~15mA

### 📡 Compatibility
- **Windows**: 10/11 (version 1903+)
- **macOS**: 10.15+
- **Linux**: BlueZ 5.50+
- **Android**: 6.0+ (API level 23+)
- **iOS**: 13.0+

## 🚀 Future Enhancements

### 🔮 Planned Features
- [ ] **🎮 Gaming Mode** - Optimized settings for gaming
- [ ] **🌐 Web Configuration** - Browser-based setup interface
- [ ] **📱 Mobile App** - Dedicated control app
- [ ] **🤖 Gesture Recognition** - Custom gesture commands
- [ ] **🔋 Power Management** - Sleep mode and battery monitoring
- [ ] **📊 Data Logging** - Movement analytics and heatmaps

### 🛠️ Hardware Upgrades
- [ ] **📡 ESP32-S3** - Enhanced processing power
- [ ] **🔘 Physical Buttons** - Additional input methods
- [ ] **🔋 LiPo Battery** - Built-in rechargeable power
- [ ] **📦 Custom PCB** - Integrated design

## 🤝 Contributing

We welcome contributions! Here's how you can help:

### 🐛 Bug Reports
1. **Check existing issues** before creating new ones
2. **Provide detailed description** including error messages
3. **Include hardware setup** and software versions
4. **Add serial output** if applicable

### 💡 Feature Requests
1. **Describe the feature** and use case
2. **Explain implementation approach** if possible
3. **Consider backward compatibility**

### 🔧 Pull Requests
1. **Fork the repository**
2. **Create feature branch** (`git checkout -b feature/amazing-feature`)
3. **Commit changes** (`git commit -m 'Add amazing feature'`)
4. **Push to branch** (`git push origin feature/amazing-feature`)
5. **Open Pull Request**

### 📝 Code Style
- Use **descriptive variable names**
- Add **comments for complex logic**
- Follow **existing code formatting**
- Update **documentation** as needed

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 Dr. Ngo Minh Tu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

## 👨‍💻 Author

- 🌐 Facebook: [NM Tu](https://www.facebook.com/daturaseeker/)
- 📧 Email: [dr.ngominhtu@gmail.com]
- 🐙 GitHub: [@ngominhtu](https://github.com/ngominhtu)


### 🏆 Acknowledgments
- **Adafruit** - MPU6050 library
- **T-vK** - ESP32 BLE Mouse library
- **Espressif** - ESP32 platform
- **PlatformIO** - Development environment

---

## 🌟 Support the Project

If this project helped you, please give it a ⭐ on GitHub!

### 📊 Project Statistics
![GitHub stars](https://img.shields.io/github/stars/drngominhtu/esp32-air-mouse?style=social)
![GitHub forks](https://img.shields.io/github/forks/drngominhtu/esp32-air-mouse?style=social)
![GitHub issues](https://img.shields.io/github/issues/drngominhtu/esp32-air-mouse)
![GitHub last commit](https://img.shields.io/github/last-commit/drngominhtu/esp32-air-mouse)
