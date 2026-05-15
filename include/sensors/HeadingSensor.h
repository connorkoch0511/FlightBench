#pragma once
#include "sensors/Sensor.h"

// Magnetic compass / heading indicator with drift and noise
class HeadingSensor : public Sensor {
public:
    explicit HeadingSensor(double noise_std_deg = 1.0, double drift_dps = 0.0,
                           unsigned seed = 44);
    double read(const FlightState& state) override;
    std::string name() const override { return "Heading Indicator"; }
    std::string unit() const override { return "deg"; }

private:
    double noise_std_;
    double drift_dps_;
    double accumulated_drift_;
    unsigned seed_;
};
