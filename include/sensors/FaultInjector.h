#pragma once
#include "sensors/Sensor.h"
#include <memory>
#include <string>

// Wraps any Sensor and injects a configurable fault during [start_s, end_s].
class FaultInjector : public Sensor {
public:
    enum class FaultType {
        STUCK,    // freezes at the reading captured at fault onset
        SPIKE,    // returns magnitude_ once at onset, then normal
        DROPOUT,  // returns 0.0 for the fault window
        DRIFT,    // adds a linearly growing bias over the fault window
    };

    FaultInjector(std::unique_ptr<Sensor> inner,
                  FaultType  type,
                  double     start_s,
                  double     end_s,
                  double     magnitude = 0.0);

    double read(const FlightState& state) override;
    std::string name() const override;
    std::string unit() const override;

    static std::string faultTypeName(FaultType t);

private:
    std::unique_ptr<Sensor> inner_;
    FaultType  type_;
    double     start_s_;
    double     end_s_;
    double     magnitude_;

    bool   fault_active_  = false;
    double stuck_value_   = 0.0;
    bool   spike_fired_   = false;
};
