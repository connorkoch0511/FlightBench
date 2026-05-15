#pragma once

struct FlightState {
    double altitude_ft      = 10000.0;
    double airspeed_kts     = 250.0;
    double heading_deg      = 0.0;
    double vertical_speed_fpm = 0.0;
    double pitch_deg        = 0.0;
    double roll_deg         = 0.0;
    double throttle_pct     = 75.0;
    double time_s           = 0.0;
};
