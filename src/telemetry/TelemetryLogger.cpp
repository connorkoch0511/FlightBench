#include "telemetry/TelemetryLogger.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

void TelemetryLogger::record(const FlightState& truth,
                              double sensor_alt, double sensor_spd, double sensor_hdg) {
    rows_.push_back({
        truth.time_s,
        truth.altitude_ft,
        truth.airspeed_kts,
        truth.heading_deg,
        truth.vertical_speed_fpm,
        truth.pitch_deg,
        truth.roll_deg,
        sensor_alt,
        sensor_spd,
        sensor_hdg
    });
}

void TelemetryLogger::clear() {
    rows_.clear();
}

void TelemetryLogger::writeCSV(const std::string& path) const {
    std::ofstream f(path);
    if (!f)
        throw std::runtime_error("Cannot open telemetry file: " + path);

    f << std::fixed << std::setprecision(4);
    f << "time_s,"
      << "true_altitude_ft,true_airspeed_kts,true_heading_deg,true_vs_fpm,"
      << "pitch_deg,roll_deg,"
      << "sensor_altitude_ft,sensor_airspeed_kts,sensor_heading_deg\n";

    for (const auto& r : rows_) {
        f << r.time_s          << ","
          << r.true_altitude_ft << "," << r.true_airspeed_kts << ","
          << r.true_heading_deg << "," << r.true_vs_fpm        << ","
          << r.pitch_deg        << "," << r.roll_deg            << ","
          << r.sensor_altitude_ft << "," << r.sensor_airspeed_kts << ","
          << r.sensor_heading_deg << "\n";
    }

    std::cout << "Telemetry written to: " << path
              << " (" << rows_.size() << " rows)\n";
}
