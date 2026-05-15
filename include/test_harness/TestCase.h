#pragma once
#include "simulation/FlightState.h"
#include "simulation/SimEngine.h"
#include "sensors/Sensor.h"
#include <string>
#include <functional>
#include <vector>
#include <memory>

// A single pass/fail criterion evaluated against a FlightState snapshot
struct Criterion {
    std::string description;
    std::function<bool(const FlightState&)> evaluate;
};

// Optional per-test sensor overrides; unset fields use the default noisy sensor
struct SensorConfig {
    std::function<std::unique_ptr<Sensor>()> make_alt;  // altitude sensor factory
    std::function<std::unique_ptr<Sensor>()> make_spd;  // airspeed sensor factory
    std::function<std::unique_ptr<Sensor>()> make_hdg;  // heading sensor factory
};

struct TestCase {
    std::string  id;
    std::string  name;
    std::string  description;
    std::string  fault_description; // empty for nominal tests
    FlightState  initial_state;
    Scenario     scenario;
    double       duration_s;
    std::vector<Criterion> criteria;   // evaluated at end of run
    std::vector<Criterion> invariants; // evaluated every step
    SensorConfig sensors;              // overrides default sensors when set
};
