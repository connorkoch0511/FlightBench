#pragma once
#include "simulation/FlightState.h"
#include <string>

class Sensor {
public:
    virtual ~Sensor() = default;
    virtual double read(const FlightState& state) = 0;
    virtual std::string name() const = 0;
    virtual std::string unit() const = 0;
};
