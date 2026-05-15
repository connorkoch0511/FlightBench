#pragma once
#include "sensors/Sensor.h"

// Pitot-static airspeed indicator with noise and optional scale factor error
class AirspeedSensor : public Sensor {
public:
    explicit AirspeedSensor(double noise_std_kts = 2.0, double scale_factor = 1.0,
                            unsigned seed = 43);
    double read(const FlightState& state) override;
    std::string name() const override { return "Airspeed Indicator"; }
    std::string unit() const override { return "kts"; }

private:
    double noise_std_;
    double scale_factor_;
    unsigned seed_;
};
