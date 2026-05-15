#include "sensors/HeadingSensor.h"
#include <random>
#include <cmath>

HeadingSensor::HeadingSensor(double noise_std_deg, double drift_dps, unsigned seed)
    : noise_std_(noise_std_deg), drift_dps_(drift_dps),
      accumulated_drift_(0.0), seed_(seed) {}

double HeadingSensor::read(const FlightState& state) {
    static std::mt19937 rng(seed_);
    std::normal_distribution<double> noise(0.0, noise_std_);
    accumulated_drift_ += drift_dps_ * 0.1; // assume 0.1s dt for drift accumulation
    double raw = state.heading_deg + accumulated_drift_ + noise(rng);
    return std::fmod(raw + 360.0, 360.0);
}
