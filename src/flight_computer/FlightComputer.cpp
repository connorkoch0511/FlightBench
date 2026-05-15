#include "flight_computer/FlightComputer.h"
#include <algorithm>
#include <cmath>

FlightComputer::FlightComputer(double target_alt, double target_hdg, double target_spd)
    : target_alt_(target_alt), target_hdg_(target_hdg), target_spd_(target_spd) {}

double FlightComputer::headingError(double current, double target) const {
    double err = target - current;
    // Normalize to [-180, 180] for shortest-path turn direction
    while (err >  180.0) err -= 360.0;
    while (err < -180.0) err += 360.0;
    return err;
}

FCOutput FlightComputer::process(const SensorPacket& packet) {
    FCOutput out{};

    // ── Altitude hold (PID) ──────────────────────────────────────────────────
    constexpr double Kp_alt = 0.8, Ki_alt = 0.01, Kd_alt = 0.3;
    double alt_error = target_alt_ - packet.altitude_ft;
    integral_alt_ = std::clamp(integral_alt_ + alt_error, -500.0, 500.0);
    double d_alt   = alt_error - prev_alt_error_;
    prev_alt_error_ = alt_error;
    double vs_cmd = Kp_alt * alt_error + Ki_alt * integral_alt_ + Kd_alt * d_alt;
    out.commanded_vs_fpm     = std::clamp(vs_cmd, -MAX_VS_FPM, MAX_VS_FPM);
    out.altitude_hold_active = (std::abs(alt_error) > 50.0);

    // ── Heading hold (P controller) ──────────────────────────────────────────
    constexpr double Kp_hdg = 1.5;
    double hdg_error = headingError(packet.heading_deg, target_hdg_);
    integral_hdg_ = std::clamp(integral_hdg_ + hdg_error, -90.0, 90.0);
    double prev_err_unused = prev_hdg_error_;
    (void)prev_err_unused;
    prev_hdg_error_ = hdg_error;
    out.commanded_heading    = std::clamp(Kp_hdg * hdg_error, -30.0, 30.0);
    out.heading_hold_active  = (std::abs(hdg_error) > 2.0);

    // ── Throttle (airspeed hold) ─────────────────────────────────────────────
    double spd_error = target_spd_ - packet.airspeed_kts;
    out.commanded_throttle = std::clamp(75.0 + spd_error * 0.5, 0.0, 100.0);

    // ── Warnings ─────────────────────────────────────────────────────────────
    out.overspeed_warning = (packet.airspeed_kts >= OVERSPEED_LIMIT_KTS);
    out.stall_warning     = (packet.airspeed_kts <= STALL_LIMIT_KTS);

    return out;
}
