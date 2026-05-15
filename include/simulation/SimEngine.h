#pragma once
#include "FlightState.h"
#include <functional>
#include <vector>
#include <string>

enum class Scenario {
    STRAIGHT_AND_LEVEL,
    CLIMB,
    DESCENT,
    TURN_LEFT,
    TURN_RIGHT,
    COMBINED  // climb + turn
};

std::string scenarioName(Scenario s);

class SimEngine {
public:
    explicit SimEngine(double dt_s = 0.1);

    void setState(const FlightState& state);
    const FlightState& getState() const;

    // Step the simulation by one dt
    void step(Scenario scenario);

    // Run for a fixed duration, calling callback each step
    void run(double duration_s, Scenario scenario,
             std::function<void(const FlightState&)> on_step = nullptr);

    double getDt() const { return dt_; }

private:
    FlightState state_;
    double dt_;

    void applyPhysics(Scenario scenario);
};
