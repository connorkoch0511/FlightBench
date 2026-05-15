#pragma once
#include "simulation/FlightState.h"
#include <string>
#include <vector>

struct TelemetryRow {
    double time_s;
    // Ground truth (from SimEngine)
    double true_altitude_ft;
    double true_airspeed_kts;
    double true_heading_deg;
    double true_vs_fpm;
    double pitch_deg;
    double roll_deg;
    // Sensor readings (noisy)
    double sensor_altitude_ft;
    double sensor_airspeed_kts;
    double sensor_heading_deg;
};

class TelemetryLogger {
public:
    void record(const FlightState& truth,
                double sensor_alt, double sensor_spd, double sensor_hdg);
    void clear();

    // Write all recorded rows to a CSV file
    void writeCSV(const std::string& path) const;

    int rowCount() const { return static_cast<int>(rows_.size()); }

private:
    std::vector<TelemetryRow> rows_;
};
