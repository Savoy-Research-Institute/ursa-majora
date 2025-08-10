# Module Guide - Ursa Minor Project

## Overview
This document defines the responsibilities of each module by stating design decisions encapsulated by it, avoiding duplication and gaps, achieving separation of concerns, and helping maintainers identify modules affected by a problem report.

## Architecture Philosophy

The Ursa Minor project follows a **layered modular architecture** inspired by the **A-7E Software Architecture** and modern software engineering principles. This architecture provides:

- **Separation of Concerns**: Each module has a single, well-defined responsibility
- **Loose Coupling**: Modules communicate through well-defined interfaces
- **High Cohesion**: Related functionality is grouped together
- **Maintainability**: Easy to understand, modify, and extend
- **Testability**: Modules can be tested independently
- **Scalability**: New modules can be added without affecting existing ones

## Module Hierarchy

```
┌─────────────────────────────────────────────────────────────┐
│                    APPLICATION LAYER                        │
│  (User Interface, Mission Planning, High-Level Control)     │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                 BEHAVIOR HIDING MODULE                      │
│  (System Behaviors, Business Logic, Function Driving)       │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                 SOFTWARE DECISION MODULE                    │
│  (Data Types, Algorithms, Physical Models, Utilities)       │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                 HARDWARE HIDING MODULE                      │
│  (Hardware Abstraction, Device Interfaces, Computer State)  │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    HARDWARE LAYER                           │
│  (Sensors, Actuators, Communication, Power Systems)         │
└─────────────────────────────────────────────────────────────┘
```

## Module Categories

### 1. Hardware Hiding Module
**Purpose**: Abstract hardware-specific details and provide clean interfaces to higher layers.

**Key Characteristics**:
- **Hardware Abstraction**: Hides hardware implementation details
- **Device Interfaces**: Provides standardized interfaces for hardware components
- **Computer State Management**: Manages system state and transitions
- **Platform Independence**: Allows software to run on different hardware platforms

**Subcategories**:

#### Device Interface Module
Provides hardware abstraction for all physical devices and sensors:
- **Air Data Interface**: Pitot tubes, static pressure sensors, temperature probes
- **Inertial Measurement Interface**: IMUs, accelerometers, gyroscopes
- **Communication Interface**: Radio modules, GPS, telemetry systems
- **Actuator Interface**: Motors, servos, ESCs
- **Sensor Interface**: Barometers, magnetometers, other sensors
- **Display Interface**: LCDs, LEDs, indicators
- **Input Interface**: Keypads, switches, buttons

#### Extended Computer Module
Manages computer state and system-level operations:
- **Computer State**: System state management and transitions
- **Interrupt Handler**: Hardware interrupt management
- **Timer Module**: System timing and scheduling
- **Memory Management**: Memory allocation and management
- **Power Management**: Power state and consumption control

**Example Implementation**:
```cpp
// Hardware Hiding - Device Interface
class AirDataInterface {
public:
    virtual float getAltitude() = 0;
    virtual float getAirspeed() = 0;
    virtual float getTemperature() = 0;
    virtual bool isHealthy() = 0;
};

// Concrete implementation hides hardware details
class BMP280AirDataInterface : public AirDataInterface {
private:
    Adafruit_BMP280 sensor;
public:
    float getAltitude() override {
        return sensor.readAltitude(SEALEVELPRESSURE_HPA);
    }
    // ... other implementations
};
```

### 2. Software Decision Module
**Purpose**: Define data types, algorithms, and business rules that are independent of hardware and behavior.

**Key Characteristics**:
- **Data Type Definitions**: Standardized data structures and types
- **Algorithm Implementation**: Mathematical and computational algorithms
- **Physical Models**: Mathematical models of physical systems
- **Utility Functions**: Common computational utilities
- **Configuration Management**: System configuration and parameters

**Subcategories**:

#### Application Data Types
Defines data structures used throughout the system:
- **Numeric Types**: Floating-point, integer types, mathematical structures
- **State Events**: System state and event definitions
- **Configuration Types**: System configuration structures
- **Communication Types**: Data packet and message structures

#### Physical Models
Mathematical models of physical systems:
- **Aircraft Motion**: Flight dynamics and aerodynamics
- **Sensor Models**: Mathematical models of sensors
- **Control Models**: PID controllers and control algorithms
- **Navigation Models**: GPS and inertial navigation algorithms

#### Software Utility Module
Common computational utilities:
- **Numerical Algorithms**: Mathematical computations
- **Filtering Algorithms**: Kalman filters, complementary filters
- **Data Processing**: Signal processing and data analysis
- **System Generation**: Code generation and configuration

#### Data Banker Module
Data management and storage:
- **Complex Events**: Event management and processing
- **Singular Values**: Single value storage and retrieval
- **Data Logging**: Flight data recording and storage
- **Configuration Storage**: System configuration persistence

**Example Implementation**:
```cpp
// Software Decision - Application Data Types
struct FlightData {
    float altitude;
    float airspeed;
    float heading;
    float pitch;
    float roll;
    float yaw;
    uint32_t timestamp;
};

// Software Decision - Physical Models
class AircraftMotion {
public:
    static float calculateLift(float airspeed, float angleOfAttack);
    static float calculateDrag(float airspeed, float angleOfAttack);
    static Vector3D calculateForces(const FlightData& data);
};
```

### 3. Behavior Hiding Module
**Purpose**: Implement system behaviors and business logic, hiding complex behaviors behind simple interfaces.

**Key Characteristics**:
- **Business Logic**: Implements system behaviors and rules
- **Function Driving**: Coordinates system functions and operations
- **Shared Services**: Common services used by multiple modules
- **State Management**: System state and mode management
- **Event Handling**: System event processing and response

**Subcategories**:

#### Function Driving Module
Implements core system functions and behaviors:
- **System Module**: Central system coordination and management
- **Flight Control Module**: Flight control and stabilization
- **Navigation Module**: Waypoint navigation and path planning
- **Communication Module**: Telemetry and command processing
- **Safety Module**: Failsafe and emergency procedures
- **Mission Module**: Mission planning and execution

#### Shared Services Module
Provides common services used across the system:
- **Shared Subroutines**: Common computational utilities
- **Mode Determination**: System mode and state determination
- **System Values**: System-wide constants and parameters
- **Event Management**: System event handling and coordination
- **Resource Management**: System resource allocation and management

**Example Implementation**:
```cpp
// Behavior Hiding - Function Driving
class FlightController {
private:
    PIDController rollPID, pitchPID, yawPID;
    FlightData currentData;
    FlightMode currentMode;
    
public:
    void update(const FlightData& sensorData) {
        currentData = sensorData;
        
        switch (currentMode) {
            case FlightMode::MANUAL:
                handleManualControl();
                break;
            case FlightMode::AUTONOMOUS:
                handleAutonomousControl();
                break;
            case FlightMode::RTH:
                handleReturnToHome();
                break;
        }
    }
    
    void setMode(FlightMode mode) {
        currentMode = mode;
        // Mode transition logic
    }
};
```

## Key Differences Between Module Types

### Hardware Hiding vs Behavior Hiding

| Aspect | Hardware Hiding | Behavior Hiding |
|--------|----------------|-----------------|
| **Purpose** | Abstract hardware details | Implement system behaviors |
| **Abstraction Level** | Low-level hardware interfaces | High-level business logic |
| **Dependencies** | Depends on actual hardware | Depends on software decisions |
| **Change Frequency** | Changes with hardware | Changes with requirements |
| **Testing** | Hardware-in-the-loop testing | Unit and integration testing |
| **Examples** | Sensor drivers, motor controllers | Flight modes, navigation logic |

### Hardware Hiding Module
- **Focus**: "How to talk to the hardware"
- **Responsibilities**: 
  - Device communication protocols
  - Hardware-specific implementations
  - Platform abstraction
  - Device state management
- **Interfaces**: Hardware-specific APIs
- **Testing**: Requires actual hardware or simulators

### Behavior Hiding Module
- **Focus**: "What the system should do"
- **Responsibilities**:
  - Business logic implementation
  - System behavior coordination
  - Mode management
  - Event handling
- **Interfaces**: High-level behavioral APIs
- **Testing**: Can be tested with mock objects

## Module Communication Patterns

### 1. Direct Interface Calls
```cpp
// Behavior Hiding calls Hardware Hiding
class FlightController {
private:
    AirDataInterface* airData;
    InertialMeasurementInterface* imu;
    
public:
    void update() {
        float altitude = airData->getAltitude();
        float pitch = imu->getPitch();
        // Process data and generate control outputs
    }
};
```

### 2. Event-Driven Communication
```cpp
// Modules communicate through events
class SystemEvent {
public:
    enum Type { SENSOR_UPDATE, MODE_CHANGE, ERROR };
    Type type;
    void* data;
};

class EventManager {
public:
    void publishEvent(const SystemEvent& event);
    void subscribe(EventType type, EventHandler handler);
};
```

### 3. Data Flow Patterns
```cpp
// Data flows from Hardware Hiding to Behavior Hiding
Hardware Layer → Hardware Hiding → Software Decision → Behavior Hiding → Application
```

## Module Implementation Guidelines

### 1. Interface Design
- **Clear Contracts**: Well-defined input/output specifications
- **Error Handling**: Comprehensive error detection and reporting
- **Documentation**: Complete API documentation
- **Versioning**: Interface versioning for backward compatibility

### 2. State Management
- **State Machines**: Clear state definitions and transitions
- **State Validation**: Input validation and state consistency
- **State Persistence**: State saving and restoration
- **State Monitoring**: State change logging and debugging

### 3. Error Handling
- **Error Codes**: Standardized error code definitions
- **Error Recovery**: Automatic error recovery procedures
- **Error Reporting**: Comprehensive error reporting and logging
- **Failsafe Mechanisms**: Safety-critical error handling

### 4. Performance Considerations
- **Real-Time Requirements**: Meeting timing constraints
- **Resource Management**: Efficient memory and CPU usage
- **Optimization**: Performance optimization where needed
- **Monitoring**: Performance monitoring and profiling

## Testing Strategy

### 1. Unit Testing
- **Hardware Hiding**: Mock hardware for testing
- **Software Decision**: Pure function testing
- **Behavior Hiding**: Mock dependencies for testing

### 2. Integration Testing
- **Module Integration**: Testing module interactions
- **System Integration**: End-to-end system testing
- **Hardware Integration**: Hardware-in-the-loop testing

### 3. System Testing
- **Flight Testing**: Real-world flight validation
- **Performance Testing**: System performance validation
- **Safety Testing**: Safety feature validation

## Maintenance and Evolution

### 1. Problem Isolation
When a problem is reported:
1. **Identify Affected Module**: Determine which module is responsible
2. **Check Module Interface**: Verify interface contract compliance
3. **Review Dependencies**: Check dependent modules
4. **Test in Isolation**: Test the specific module
5. **Validate Integration**: Test module integration

### 2. Module Evolution
- **Interface Stability**: Maintain backward compatibility
- **Version Management**: Proper versioning and migration
- **Documentation Updates**: Keep documentation current
- **Testing Updates**: Update tests with changes

### 3. Adding New Modules
1. **Define Interface**: Clear interface specification
2. **Implement Module**: Complete module implementation
3. **Add Tests**: Comprehensive test coverage
4. **Update Documentation**: Complete documentation
5. **Integration**: Integrate with existing modules

## Conclusion

This modular architecture provides a solid foundation for the Ursa Minor project, enabling:

- **Maintainability**: Clear separation of concerns
- **Testability**: Independent module testing
- **Scalability**: Easy addition of new features
- **Reliability**: Robust error handling and safety
- **Performance**: Optimized for real-time operation

The key to success is maintaining the architectural boundaries and ensuring each module fulfills its specific responsibilities while providing clean, well-documented interfaces to other modules. 