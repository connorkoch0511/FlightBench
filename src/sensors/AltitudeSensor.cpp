#include "sensors/AltitudeSensor.h"
#include <random>

AltitudeSensor::AltitudeSensor(double noise_std_ft, double bias_ft, unsigned seed)
    : noise_std_(noise_std_ft), bias_(bias_ft), seed_(seed) {}

double AltitudeSensor::read(const FlightState& state) {
    static std::mt19937 rng(seed_);
    std::normal_distribution<double> noise(0.0, noise_std_);
    return state.altitude_ft + bias_ + noise(rng);
}
