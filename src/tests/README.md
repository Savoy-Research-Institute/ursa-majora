# Test Suite Organization

This directory contains all tests for the Ursa Minor drone system, organized by category and purpose.

## Directory Structure

```
src/tests/
├── README.md                    # This file - test organization guide
├── unit_tests/                  # Unit tests for individual modules
├── integration_tests/           # Integration tests for module interactions
├── hardware_tests/             # Hardware-specific tests and calibration
├── flight_tests/               # Flight control and safety tests
└── test_utils/                 # Common testing utilities and helpers
```

## Test Categories

### Unit Tests (`unit_tests/`)
- **Purpose**: Test individual modules in isolation
- **Scope**: Single module functionality, methods, and data structures
- **Dependencies**: Minimal, mock external dependencies when possible
- **Examples**: Alert module tests, GPS interface tests, sensor validation tests

### Integration Tests (`integration_tests/`)
- **Purpose**: Test module interactions and system behavior
- **Scope**: Multiple modules working together, data flow, communication
- **Dependencies**: Multiple modules, but still controlled environment
- **Examples**: Communication between console and flight controller, sensor data pipeline

### Hardware Tests (`hardware_tests/`)
- **Purpose**: Test actual hardware components and calibration
- **Scope**: Physical sensors, actuators, communication devices
- **Dependencies**: Real hardware, may require specific test setup
- **Examples**: MPU6050 calibration, ESC testing, GPS signal testing

### Flight Tests (`flight_tests/`)
- **Purpose**: Test flight control algorithms and safety systems
- **Scope**: Flight modes, control loops, failsafe mechanisms
- **Dependencies**: Flight controller, safety systems, simulation environment
- **Examples**: PID tuning tests, autonomous flight tests, emergency procedures

### Test Utilities (`test_utils/`)
- **Purpose**: Common testing functions and mock objects
- **Scope**: Reusable testing code, test data generators, assertions
- **Dependencies**: Arduino framework, testing libraries
- **Examples**: Mock sensor data, test assertions, performance measurement

## Test File Naming Convention

- **Unit tests**: `{module_name}_unit_test.cpp`
- **Integration tests**: `{feature_name}_integration_test.cpp`
- **Hardware tests**: `{component_name}_hardware_test.ino`
- **Flight tests**: `{flight_mode}_flight_test.ino`
- **Test utilities**: `test_{utility_name}.cpp`

## Running Tests

### Individual Tests
```bash
# Compile and upload specific test
arduino-cli compile --fqbn arduino:avr:mega2560 src/tests/unit_tests/alert_unit_test.cpp
arduino-cli upload --fqbn arduino:avr:mega2560 --port /dev/ttyUSB0
```

### Test Suites
```bash
# Run all unit tests
find src/tests/unit_tests/ -name "*.cpp" -exec arduino-cli compile --fqbn arduino:avr:mega2560 {} \;

# Run all hardware tests
find src/tests/hardware_tests/ -name "*.ino" -exec arduino-cli compile --fqbn arduino:avr:mega2560 {} \;
```

## Test Development Guidelines

1. **Isolation**: Each test should be independent and not affect others
2. **Clear Naming**: Test names should clearly indicate what is being tested
3. **Documentation**: Include setup requirements and expected results
4. **Error Handling**: Tests should gracefully handle hardware failures
5. **Performance**: Tests should complete in reasonable time
6. **Safety**: Hardware tests should include safety checks and limits

## Test Data and Mocking

- Use `test_utils/` for common test data and mock objects
- Mock external dependencies (sensors, communication) when testing logic
- Provide realistic test data that represents actual operating conditions
- Include edge cases and error conditions in test scenarios

## Continuous Integration

- Tests should be runnable in automated CI/CD pipelines
- Hardware tests can be run on dedicated test hardware
- Unit and integration tests should run on any Arduino-compatible board
- Test results should be reportable and actionable 