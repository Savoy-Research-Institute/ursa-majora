#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include <Arduino.h>
#include "hardware_hiding/device_interface/air_data_interface.h"
#include "hardware_hiding/device_interface/audible_signal_interface.h"
#include "hardware_hiding/device_interface/inertial_measurement_interface.h"
#include "hardware_hiding/device_interface/radar_interface.h"
#include "hardware_hiding/device_interface/weapons_interface.h"
#include "hardware_hiding/device_interface/panel_interface.h"
#include "hardware_hiding/extended_computer/computer_state.h"
#include "hardware_hiding/extended_computer/interrupt_handler.h"
#include "hardware_hiding/extended_computer/timer_module.h"
#include "behavior_hiding/function_driving/system_module.h"
#include "behavior_hiding/function_driving/audible_signal_module.h"
#include "behavior_hiding/function_driving/visual_indicator_module.h"
#include "behavior_hiding/function_driving/inertial_navigation_module.h"
#include "behavior_hiding/function_driving/air_data_computer.h"
#include "behavior_hiding/shared_services/mode_determination.h"
#include "behavior_hiding/shared_services/system_values.h"
#include "software_decision/application_data_types/numeric_types.h"
#include "software_decision/application_data_types/state_events.h"
#include "software_decision/physical_models/aircraft_motion.h"
#include "software_decision/physical_models/earth_characteristics.h"
#include "software_decision/software_utility/power_initialization.h"
#include "software_decision/software_utility/numerical_algorithms.h"

// Module Manager - Central coordination point for all aircraft systems
class ModuleManager {
private:
    // Hardware Hiding Modules
    AirDataInterface airDataInterface;
    AudibleSignalInterface audibleSignalInterface;
    InertialMeasurementInterface inertialMeasurementInterface;
    RadarInterface radarInterface;
    WeaponsInterface weaponsInterface;
    PanelInterface panelInterface;
    
    // Extended Computer Modules
    ComputerState computerState;
    InterruptHandler interruptHandler;
    TimerModule timerModule;
    
    // Behavior Hiding Modules
    SystemModule systemModule;
    AudibleSignalModule audibleSignalModule;
    VisualIndicatorModule visualIndicatorModule;
    InertialNavigationModule inertialNavigationModule;
    AirDataComputer airDataComputer;
    
    // Shared Services
    ModeDetermination modeDetermination;
    SystemValues systemValues;
    
    // Software Decision Modules
    NumericTypes numericTypes;
    StateEvents stateEvents;
    AircraftMotion aircraftMotion;
    EarthCharacteristics earthCharacteristics;
    PowerInitialization powerInitialization;
    NumericalAlgorithms numericalAlgorithms;
    
    bool systemInitialized;
    unsigned long lastUpdateTime;
    
public:
    ModuleManager();
    
    // System lifecycle management
    bool initialize();
    void update();
    void shutdown();
    
    // Module access methods
    AirDataInterface* getAirDataInterface() { return &airDataInterface; }
    AudibleSignalInterface* getAudibleSignalInterface() { return &audibleSignalInterface; }
    InertialMeasurementInterface* getInertialMeasurementInterface() { return &inertialMeasurementInterface; }
    RadarInterface* getRadarInterface() { return &radarInterface; }
    WeaponsInterface* getWeaponsInterface() { return &weaponsInterface; }
    PanelInterface* getPanelInterface() { return &panelInterface; }
    
    SystemModule* getSystemModule() { return &systemModule; }
    AudibleSignalModule* getAudibleSignalModule() { return &audibleSignalModule; }
    VisualIndicatorModule* getVisualIndicatorModule() { return &visualIndicatorModule; }
    InertialNavigationModule* getInertialNavigationModule() { return &inertialNavigationModule; }
    AirDataComputer* getAirDataComputer() { return &airDataComputer; }
    
    // System status
    bool isInitialized() const { return systemInitialized; }
    void getSystemStatus(String& status);
    
    // Emergency handling
    void handleEmergency();
    void performSystemCheck();
};

extern ModuleManager moduleManager;

#endif // MODULE_MANAGER_H 