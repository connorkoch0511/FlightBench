#include "sensors/FaultInjector.h"
#include <algorithm>

FaultInjector::FaultInjector(std::unique_ptr<Sensor> inner,
                              FaultType type, double start_s,
                              double end_s, double magnitude)
    : inner_(std::move(inner)), type_(type),
      start_s_(start_s), end_s_(end_s), magnitude_(magnitude) {}

std::string FaultInjector::name() const {
    return inner_->name() + " [" + faultTypeName(type_) + " FAULT]";
}

std::string FaultInjector::unit() const { return inner_->unit(); }

std::string FaultInjector::faultTypeName(FaultType t) {
    switch (t) {
        case FaultType::STUCK:   return "STUCK";
        case FaultType::SPIKE:   return "SPIKE";
        case FaultType::DROPOUT: return "DROPOUT";
        case FaultType::DRIFT:   return "DRIFT";
    }
    return "UNKNOWN";
}

double FaultInjector::read(const FlightState& state) {
    double clean = inner_->read(state);
    bool in_window = (state.time_s >= start_s_ && state.time_s <= end_s_);

    if (!in_window) {
        fault_active_ = false;
        spike_fired_  = false;
        stuck_value_  = clean; // track last clean value for STUCK onset
        return clean;
    }

    // First call inside the fault window
    if (!fault_active_) {
        fault_active_ = true;
        stuck_value_  = clean;
    }

    switch (type_) {
        case FaultType::STUCK:
            return stuck_value_;

        case FaultType::SPIKE:
            if (!spike_fired_) {
                spike_fired_ = true;
                return stuck_value_ + magnitude_;
            }
            return clean;

        case FaultType::DROPOUT:
            return 0.0;

        case FaultType::DRIFT: {
            double progress = (state.time_s - start_s_) / (end_s_ - start_s_);
            return clean + magnitude_ * progress;
        }
    }
    return clean;
}
