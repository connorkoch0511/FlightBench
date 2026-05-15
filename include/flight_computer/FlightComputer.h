#pragma once
#include <string>

struct SensorPacket {
    double altitude_ft;
    double airspeed_kts;
    double heading_deg;
    double time_s;
};

struct FCOutput {
    double commanded_vs_fpm;    // vertical speed command
    double commanded_heading;   // heading hold output
    double commanded_throttle;  // 0-100%
    bool   altitude_hold_active;
    bool   heading_hold_active;
    bool   overspeed_warning;
    bool   stall_warning;
};

class FlightComputer {
public:
    FlightComputer(double target_altitude_ft  = 10000.0,
                   double target_heading_deg  = 0.0,
                   double target_airspeed_kts = 250.0);

    FCOutput process(const SensorPacket& packet);

    void setTargetAltitude(double ft)   { target_alt_  = ft; }
    void setTargetHeading(double deg)   { target_hdg_  = deg; }
    void setTargetAirspeed(double kts)  { target_spd_  = kts; }

    double getTargetAltitude()  const { return target_alt_; }
    double getTargetHeading()   const { return target_hdg_; }
    double getTargetAirspeed()  const { return target_spd_; }

private:
    double target_alt_;
    double target_hdg_;
    double target_spd_;

    double prev_alt_error_  = 0.0;
    double integral_alt_    = 0.0;
    double prev_hdg_error_  = 0.0;
    double integral_hdg_    = 0.0;

    static constexpr double OVERSPEED_LIMIT_KTS = 340.0;
    static constexpr double STALL_LIMIT_KTS     = 130.0;
    static constexpr double MAX_VS_FPM          = 2500.0;

    double headingError(double current, double target) const;
};
