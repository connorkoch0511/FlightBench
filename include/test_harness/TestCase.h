#pragma once
#include "simulation/FlightState.h"
#include "simulation/SimEngine.h"
#include <string>
#include <functional>
#include <vector>

// A single pass/fail criterion evaluated against a FlightState snapshot
struct Criterion {
    std::string description;
    std::function<bool(const FlightState&)> evaluate;
};

struct TestCase {
    std::string  id;
    std::string  name;
    std::string  description;
    FlightState  initial_state;
    Scenario     scenario;
    double       duration_s;
    std::vector<Criterion> criteria;  // evaluated at end of run
    std::vector<Criterion> invariants; // evaluated every step
};
