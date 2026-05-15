#pragma once
#include "sensors/Sensor.h"

// Barometric altimeter with configurable noise and bias
class AltitudeSensor : public Sensor {
public:
    explicit AltitudeSensor(double noise_std_ft = 15.0, double bias_ft = 0.0,
                            unsigned seed = 42);
    double read(const FlightState& state) override;
    std::string name() const override { return "Altimeter"; }
    std::string unit() const override { return "ft"; }

private:
    double noise_std_;
    double bias_;
    unsigned seed_;
};
