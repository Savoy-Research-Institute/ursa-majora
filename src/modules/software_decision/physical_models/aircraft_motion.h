#ifndef AIRCRAFT_MOTION_H
#define AIRCRAFT_MOTION_H

#include <Arduino.h>
#include "../application_data_types/numeric_types.h"
#include "../software_utility/numerical_algorithms.h"

// Flight regimes
enum class FlightRegime {
    GROUND,
    TAKEOFF,
    CLIMB,
    CRUISE,
    DESCENT,
    APPROACH,
    LANDING,
    MANEUVER
};

// Aircraft configuration
enum class AircraftConfiguration {
    CLEAN,
    FLAPS_EXTENDED,
    GEAR_DOWN,
    SPOILERS_DEPLOYED,
    WEAPONS_LOADED,
    FUEL_TANKS_EMPTY,
    CARGO_LOADED
};

// Atmospheric conditions
enum class AtmosphericCondition {
    STANDARD_DAY,
    HOT_DAY,
    COLD_DAY,
    HIGH_ALTITUDE,
    TURBULENT,
    ICING_CONDITIONS
};

// Force components
struct ForceComponents {
    float lift;          // Lift force (N)
    float drag;          // Drag force (N)
    float thrust;        // Thrust force (N)
    float weight;        // Weight force (N)
    float sideForce;     // Side force (N)
    
    // Total force
    float totalForce() const { return sqrt(lift*lift + drag*drag + thrust*thrust + weight*weight + sideForce*sideForce); }
};

// Moment components
struct MomentComponents {
    float rollMoment;    // Roll moment (N⋅m)
    float pitchMoment;   // Pitch moment (N⋅m)
    float yawMoment;     // Yaw moment (N⋅m)
    
    // Total moment
    float totalMoment() const { return sqrt(rollMoment*rollMoment + pitchMoment*pitchMoment + yawMoment*yawMoment); }
};

// Aircraft state
struct AircraftState {
    // Position (NED coordinates)
    float north;         // North position (m)
    float east;          // East position (m)
    float down;          // Down position (m)
    
    // Velocity (NED coordinates)
    float northVelocity; // North velocity (m/s)
    float eastVelocity;  // East velocity (m/s)
    float downVelocity;  // Down velocity (m/s)
    
    // Attitude (Euler angles)
    float roll;          // Roll angle (rad)
    float pitch;         // Pitch angle (rad)
    float yaw;           // Yaw angle (rad)
    
    // Angular rates
    float rollRate;      // Roll rate (rad/s)
    float pitchRate;     // Pitch rate (rad/s)
    float yawRate;       // Yaw rate (rad/s)
    
    // Air data
    float airspeed;      // True airspeed (m/s)
    float altitude;      // Altitude (m)
    float angleOfAttack; // Angle of attack (rad)
    float sideslipAngle; // Sideslip angle (rad)
    
    // Time
    unsigned long timestamp;
};

// Aircraft parameters
struct AircraftParameters {
    // Mass properties
    float mass;          // Aircraft mass (kg)
    float momentOfInertiaXX; // Roll moment of inertia (kg⋅m²)
    float momentOfInertiaYY; // Pitch moment of inertia (kg⋅m²)
    float momentOfInertiaZZ; // Yaw moment of inertia (kg⋅m²)
    float momentOfInertiaXZ; // Cross moment of inertia (kg⋅m²)
    
    // Geometric properties
    float wingArea;      // Wing area (m²)
    float wingSpan;      // Wing span (m)
    float meanAerodynamicChord; // Mean aerodynamic chord (m)
    float aspectRatio;   // Wing aspect ratio
    
    // Aerodynamic coefficients
    float cl0;           // Zero-lift coefficient
    float clAlpha;       // Lift curve slope (1/rad)
    float clDeltaE;      // Elevator effectiveness (1/rad)
    float clDeltaF;      // Flap effectiveness (1/rad)
    
    float cd0;           // Zero-drag coefficient
    float cdAlpha;       // Drag curve slope (1/rad)
    float cdDeltaE;      // Elevator drag coefficient (1/rad)
    float cdDeltaF;      // Flap drag coefficient (1/rad)
    
    float cm0;           // Zero-pitch moment coefficient
    float cmAlpha;       // Pitch moment curve slope (1/rad)
    float cmDeltaE;      // Elevator moment coefficient (1/rad)
    float cmDeltaF;      // Flap moment coefficient (1/rad)
    
    // Control surface effectiveness
    float deltaE;        // Elevator deflection (rad)
    float deltaA;        // Aileron deflection (rad)
    float deltaR;        // Rudder deflection (rad)
    float deltaF;        // Flap deflection (rad)
    float deltaT;        // Throttle setting (0-1)
};

// Atmospheric model
struct AtmosphericModel {
    float temperature;   // Temperature (K)
    float pressure;      // Pressure (Pa)
    float density;       // Air density (kg/m³)
    float viscosity;     // Dynamic viscosity (Pa⋅s)
    float speedOfSound;  // Speed of sound (m/s)
    
    // Wind components
    float windNorth;     // North wind component (m/s)
    float windEast;      // East wind component (m/s)
    float windDown;      // Down wind component (m/s)
    
    // Turbulence
    float turbulenceIntensity; // Turbulence intensity (0-1)
    float gustIntensity;       // Gust intensity (m/s)
};

// Flight dynamics model
struct FlightDynamicsModel {
    // Stability derivatives
    float xu, xw, xq, xdeltaE, xdeltaT;
    float zu, zw, zq, zdeltaE, zdeltaT;
    float mu, mw, mq, mdeltaE, mdeltaT;
    float yu, yv, yp, yr, ydeltaA, ydeltaR;
    float lu, lv, lp, lr, ldeltaA, ldeltaR;
    float nu, nv, np, nr, ndeltaA, ndeltaR;
    
    // Control derivatives
    float xdeltaA, xdeltaR;
    float zdeltaA, zdeltaR;
    float mdeltaA, mdeltaR;
};

// Aircraft motion configuration
struct AircraftMotionConfig {
    uint16_t maxStates;
    uint16_t updateInterval;
    bool enableAerodynamicModel;
    bool enablePropulsionModel;
    bool enableGravityModel;
    bool enableAtmosphericModel;
    bool enableTurbulenceModel;
    bool enableControlModel;
    uint16_t integrationSteps;
    float integrationStepSize;
};

class AircraftMotion {
private:
    // Configuration
    AircraftMotionConfig config;
    
    // Aircraft parameters
    AircraftParameters aircraftParams;
    
    // Atmospheric model
    AtmosphericModel atmosphere;
    
    // Flight dynamics model
    FlightDynamicsModel flightDynamics;
    
    // Current state
    AircraftState currentState;
    
    // State history
    AircraftState* stateHistory;
    uint16_t stateHistorySize;
    uint16_t stateHistoryIndex;
    
    // Numerical algorithms
    NumericalAlgorithms* numericalAlgorithms;
    
    // Error tracking
    uint8_t errorFlags;
    String lastError;
    
    // Helper methods
    void updateAtmosphericModel(float altitude);
    ForceComponents calculateAerodynamicForces(const AircraftState& state);
    MomentComponents calculateAerodynamicMoments(const AircraftState& state);
    ForceComponents calculatePropulsionForces(const AircraftState& state);
    MomentComponents calculatePropulsionMoments(const AircraftState& state);
    ForceComponents calculateGravityForces(const AircraftState& state);
    
    void integrateState(AircraftState& state, float dt);
    void applyControlInputs(const AircraftParameters& params);
    void applyAtmosphericEffects(AircraftState& state);
    void applyTurbulence(AircraftState& state);
    
    bool validateState(const AircraftState& state) const;
    bool validateParameters(const AircraftParameters& params) const;
    void setError(const String& error);

public:
    AircraftMotion();
    ~AircraftMotion();
    
    // Initialization and configuration
    bool initialize(const AircraftMotionConfig& config = AircraftMotionConfig{});
    void setConfiguration(const AircraftMotionConfig& newConfig);
    AircraftMotionConfig getConfiguration() const { return config; }
    
    // Aircraft parameter management
    bool setAircraftParameters(const AircraftParameters& params);
    AircraftParameters getAircraftParameters() const { return aircraftParams; }
    
    bool setMass(float mass);
    bool setMomentOfInertia(float Ixx, float Iyy, float Izz, float Ixz = 0.0f);
    bool setWingGeometry(float area, float span, float mac);
    bool setAerodynamicCoefficients(float cl0, float clAlpha, float cd0, float cm0);
    
    // Atmospheric model management
    bool setAtmosphericConditions(const AtmosphericModel& conditions);
    bool setAltitude(float altitude);
    bool setTemperature(float temperature);
    bool setPressure(float pressure);
    bool setWindComponents(float north, float east, float down);
    bool setTurbulence(float intensity, float gustIntensity);
    
    // Flight dynamics model management
    bool setFlightDynamicsModel(const FlightDynamicsModel& model);
    bool setStabilityDerivatives(const FlightDynamicsModel& derivatives);
    bool setControlDerivatives(const FlightDynamicsModel& derivatives);
    
    // State management
    bool setAircraftState(const AircraftState& state);
    AircraftState getAircraftState() const { return currentState; }
    
    bool setPosition(float north, float east, float down);
    bool setVelocity(float north, float east, float down);
    bool setAttitude(float roll, float pitch, float yaw);
    bool setAngularRates(float rollRate, float pitchRate, float yawRate);
    bool setAirspeed(float airspeed);
    bool setAltitude(float altitude);
    
    // Control input management
    bool setControlInputs(float elevator, float aileron, float rudder, float flaps, float throttle);
    bool setElevator(float deflection);
    bool setAileron(float deflection);
    bool setRudder(float deflection);
    bool setFlaps(float deflection);
    bool setThrottle(float setting);
    
    // Flight dynamics calculations
    ForceComponents calculateTotalForces(const AircraftState& state);
    MomentComponents calculateTotalMoments(const AircraftState& state);
    
    float calculateLiftCoefficient(float angleOfAttack, float flapDeflection);
    float calculateDragCoefficient(float angleOfAttack, float flapDeflection, float liftCoefficient);
    float calculatePitchMomentCoefficient(float angleOfAttack, float elevatorDeflection, float flapDeflection);
    
    // Flight performance calculations
    float calculateStallSpeed(float altitude, float flapDeflection);
    float calculateMaximumLiftCoefficient(float flapDeflection);
    float calculateDragPolar(float angleOfAttack, float flapDeflection);
    float calculateLiftToDragRatio(float angleOfAttack, float flapDeflection);
    
    // Stability analysis
    bool analyzeLongitudinalStability(float& naturalFrequency, float& dampingRatio);
    bool analyzeLateralStability(float& naturalFrequency, float& dampingRatio);
    bool analyzeDirectionalStability(float& naturalFrequency, float& dampingRatio);
    
    // Flight envelope
    bool calculateFlightEnvelope(float* altitude, float* airspeed, uint16_t& count);
    bool isWithinFlightEnvelope(float altitude, float airspeed) const;
    bool calculateServiceCeiling(float& ceiling);
    bool calculateMaximumRange(float& range);
    bool calculateMaximumEndurance(float& endurance);
    
    // Flight simulation
    bool simulateFlight(float duration, float timeStep);
    bool simulateManeuver(const String& maneuverType, float duration);
    bool simulateTakeoff(float runwayLength, float& takeoffDistance);
    bool simulateLanding(float approachSpeed, float& landingDistance);
    
    // State prediction
    AircraftState predictState(float timeAhead) const;
    bool predictCollision(const AircraftState& otherAircraft, float& timeToCollision);
    bool predictStall(float& timeToStall);
    bool predictSpin(float& timeToSpin);
    
    // System operations
    void update();
    void shutdown();
    bool performSelfTest();
    
    // Status and monitoring
    bool isHealthy() const;
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Utility methods
    void printAircraftParameters();
    void printCurrentState();
    void printFlightEnvelope();
    void printConfiguration();
    void printStatistics();
    
    // Data export
    bool exportFlightData(const String& filename);
    bool exportStateHistory(const String& filename);
    bool exportPerformanceData(const String& filename);
    
    // Performance optimization
    bool optimizeAerodynamics();
    bool optimizeControlSystem();
    bool optimizeFlightPath();
    
    // Setter methods for numerical algorithms
    void setNumericalAlgorithms(NumericalAlgorithms* algorithms) { numericalAlgorithms = algorithms; }
};

#endif // AIRCRAFT_MOTION_H 