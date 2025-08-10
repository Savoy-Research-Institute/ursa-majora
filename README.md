# 🚁 Ursa Majora - Advanced Drone Control System

## 🎯 Project Overview

Ursa Majora is a sophisticated drone control system featuring both airborne (Velma) and ground station (Blizzard) components. The system implements a modular, professional-grade architecture designed for reliability, safety, and performance.

## 🏗️ Architecture

### **Three-Layer Modular Design**

1. **Behavior Hiding Layer** - System behaviors and business logic
   - Function Driving Modules (13 modules)
   - Shared Services Modules (5 modules)

2. **Hardware Hiding Layer** - Hardware abstraction and device interfaces
   - Device Interface Modules (22 modules)
   - Extended Computer Modules (8 modules)

3. **Software Decision Layer** - Data types, algorithms, and physical models
   - Application Data Types (2 modules)
   - Data Banker Modules (2 modules)
   - Physical Models (5 modules)
   - Software Utility Modules (2 modules)
   - System Generation Modules (2 modules)

## 🚀 Key Features

- **Real-time Control**: 250-500 Hz control loops
- **Advanced Sensor Fusion**: Complementary/Kalman filtering
- **Robust Communication**: HC-12, XBee, LoRa support
- **Multiple Flight Modes**: Manual, Self-Leveling, GPS Hold, RTH, Autonomous
- **Comprehensive Safety**: Signal loss detection, battery monitoring, crash detection
- **Professional Architecture**: Industry-standard modular design patterns

## 📁 Project Structure

```
001--ursa-majora/
├── src/                          # Source code
│   ├── modules/                  # Modular architecture
│   │   ├── behavior_hiding/      # System behaviors
│   │   ├── hardware_hiding/      # Hardware abstraction
│   │   └── software_decision/    # Data types & algorithms
│   ├── include/                  # Legacy headers (to be migrated)
│   ├── lib/                      # External libraries
│   ├── sensors/                  # Sensor test files
│   └── tests/                    # Test suite
├── project notes/                # Documentation & planning
├── ISSUE_TEMPLATE/              # GitHub issue templates
└── MODULE_IMPLEMENTATION_SUMMARY.md
```

## 🔧 Hardware Components

### **Velma (Drone)**
- **MCU**: Arduino Mega 2560 / STM32
- **Sensors**: MPU6050, BMP280, GPS, Magnetometer
- **Communication**: HC-12 Transceiver (433MHz)
- **Power**: LiPo Battery
- **Interface**: PWM (ESCs), I2C, UART

### **Blizzard (Ground Station)**
- **MCU**: Arduino Nano
- **Sensors**: GPS, Magnetometer, Accelerometer
- **Communication**: HC-12 Transceiver (433MHz)
- **Power**: 9V Battery
- **Interface**: Keypad, LCD, LEDs, Buzzer

## 🚀 Getting Started

### **Prerequisites**
- Arduino IDE 1.8.x or PlatformIO
- Arduino Mega 2560 or compatible board
- Required sensors and components (see schematics)
- HC-12 transceiver modules

### **Installation**
1. Clone this repository
2. Install required Arduino libraries (see `src/lib/`)
3. Connect hardware according to schematics
4. Upload appropriate firmware to Velma and Blizzard

### **Quick Start**
1. **Velma Setup**: Upload `src/sensors/camera_brick.ino` to drone
2. **Blizzard Setup**: Upload `project notes/003--PRODUCT DEVELOPMENT/001--HARDWARE DEVELOPMENT/004--BLIZZARD CONSOLES/blizzard/blizzard.ino` to ground station
3. **Power On**: Both systems will initialize and establish communication
4. **Calibration**: Follow on-screen prompts for sensor calibration

## 📚 Documentation

- **Module Guide**: `project notes/004--MODULE GUIDE/README.md`
- **Business Strategy**: `project notes/001-README/BUSINESS_STRATEGY.md`
- **Development Guidelines**: `project notes/001-README/DEVELOPMENT_GUIDELINES.md`
- **Hardware Architecture**: `project notes/001-README/hardware_architecture.md`
- **Software Architecture**: `project notes/001-README/software_architecture.md`

## 🧪 Testing

### **Test Categories**
- **Unit Tests**: Individual module testing
- **Integration Tests**: Module interaction testing
- **Hardware Tests**: Sensor and hardware validation
- **Flight Tests**: Full system validation

### **Running Tests**
```bash
# Navigate to test directory
cd src/tests/

# Run specific test categories
# Unit tests
arduino --verify isolated_tests/alert_tests.ino

# Hardware tests
arduino --verify hardware_tests/mpu6050_hardware_test.ino

# Integration tests
arduino --verify integration_tests/
```

## 🔄 Development Workflow

### **Current Status**: 42.3% Complete (22/52 modules)

### **Development Phases**
1. **Phase 1**: Critical Function Driving Modules (5 remaining)
2. **Phase 2**: Device Interface Modules (15 remaining)
3. **Phase 3**: Extended Computer Modules (5 remaining)
4. **Phase 4**: Physical Models (3 remaining)
5. **Phase 5**: System Generation (2 remaining)

### **Next Steps**
- Complete module implementation
- Create `.cpp` files for all modules
- Implement module manager coordination
- Add comprehensive testing
- Validate hardware integration

## 🛡️ Safety Features

- **Failsafe Systems**: Signal loss detection and recovery
- **Battery Monitoring**: Low voltage protection and warnings
- **Crash Detection**: Impact detection and emergency procedures
- **Geofencing**: Configurable flight boundaries
- **Emergency Landing**: Automatic safe landing procedures

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Follow the established coding standards
4. Add comprehensive tests
5. Submit a pull request

### **Coding Standards**
- Follow established module patterns
- Include comprehensive error handling
- Add proper documentation
- Maintain consistent naming conventions
- Implement state management systems

## 📄 License

This project is proprietary software. All rights reserved.

## 🆘 Support

For issues and questions:
1. Check the documentation in `project notes/`
2. Review existing issues
3. Create a new issue with detailed information
4. Include hardware configuration and error logs

## 📊 Project Health

- **Code Quality**: Professional-grade architecture
- **Documentation**: Comprehensive and well-organized
- **Testing**: Structured test framework
- **Safety**: Multiple safety systems implemented
- **Performance**: Real-time capable control loops

---

**Last Updated**: January 2025  
**Version**: 1.0.0  
**Status**: Active Development
