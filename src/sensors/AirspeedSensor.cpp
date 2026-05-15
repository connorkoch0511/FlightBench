#include "sensors/AirspeedSensor.h"
#include <random>

AirspeedSensor::AirspeedSensor(double noise_std_kts, double scale_factor, unsigned seed)
    : noise_std_(noise_std_kts), scale_factor_(scale_factor), seed_(seed) {}

double AirspeedSensor::read(const FlightState& state) {
    static std::mt19937 rng(seed_);
    std::normal_distribution<double> noise(0.0, noise_std_);
    return state.airspeed_kts * scale_factor_ + noise(rng);
}
