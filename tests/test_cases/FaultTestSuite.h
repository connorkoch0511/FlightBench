#pragma once
#include "test_harness/TestCase.h"
#include "sensors/AltitudeSensor.h"
#include "sensors/AirspeedSensor.h"
#include "sensors/HeadingSensor.h"
#include "sensors/FaultInjector.h"
#include <vector>
#include <memory>
#include <cmath>

inline std::vector<TestCase> buildFaultTestSuite() {
    std::vector<TestCase> suite;

    // ── TC-F01: Stuck altimeter during climb ──────────────────────────────────
    // Altimeter freezes at the initial reading for 20 seconds mid-climb.
    // Truth: aircraft climbs. Sensor: flat line during fault window.
    // Verification: true altitude still gains ≥500ft despite stuck sensor.
    {
        TestCase tc;
        tc.id   = "TC-F01";
        tc.name = "Stuck Altimeter During Climb";
        tc.description =
            "Altimeter freezes at initial reading for 20s mid-climb. "
            "Aircraft must still gain altitude despite faulty sensor.";
        tc.fault_description =
            "AltitudeSensor STUCK fault: t=10s–30s. "
            "Sensor returns the value captured at fault onset.";
        tc.scenario   = Scenario::CLIMB;
        tc.duration_s = 60.0;
        tc.initial_state.altitude_ft  = 8000.0;
        tc.initial_state.airspeed_kts = 240.0;

        tc.sensors.make_alt = []() {
            return std::make_unique<FaultInjector>(
                std::make_unique<AltitudeSensor>(),
                FaultInjector::FaultType::STUCK,
                10.0, 30.0
            );
        };

        tc.invariants.push_back({
            "Airspeed must remain above stall speed",
            [](const FlightState& s) { return s.airspeed_kts > 130.0; }
        });
        tc.criteria.push_back({
            "True altitude gained ≥500ft despite stuck altimeter",
            [](const FlightState& s) { return s.altitude_ft >= 8500.0; }
        });
        tc.criteria.push_back({
            "True altitude did not exceed 15000ft (no runaway climb)",
            [](const FlightState& s) { return s.altitude_ft <= 15000.0; }
        });
        suite.push_back(tc);
    }

    // ── TC-F02: Airspeed spike (pitot static anomaly) ────────────────────────
    // Single spike of +150kts reported at t=5s (reads 390kts → above VMO 340).
    // Verification: spike is brief, airspeed returns to normal, no stall.
    {
        TestCase tc;
        tc.id   = "TC-F02";
        tc.name = "Airspeed Spike — Pitot Anomaly";
        tc.description =
            "A single +150kt spike in reported airspeed at t=5s simulates "
            "a pitot tube transient. True airspeed unaffected.";
        tc.fault_description =
            "AirspeedSensor SPIKE fault: t=5s, magnitude=+150kts. "
            "Sensor reports 390+kts for one sample, then returns to normal.";
        tc.scenario   = Scenario::STRAIGHT_AND_LEVEL;
        tc.duration_s = 60.0;
        tc.initial_state.altitude_ft  = 10000.0;
        tc.initial_state.airspeed_kts = 240.0;

        tc.sensors.make_spd = []() {
            return std::make_unique<FaultInjector>(
                std::make_unique<AirspeedSensor>(),
                FaultInjector::FaultType::SPIKE,
                5.0, 5.5,
                150.0  // +150kts spike magnitude
            );
        };

        tc.invariants.push_back({
            "True airspeed must stay within normal range (130–340kts)",
            [](const FlightState& s) {
                return s.airspeed_kts > 130.0 && s.airspeed_kts < 340.0;
            }
        });
        tc.criteria.push_back({
            "True airspeed returned to near-normal (200–280kts) after spike",
            [](const FlightState& s) {
                return s.airspeed_kts >= 200.0 && s.airspeed_kts <= 280.0;
            }
        });
        tc.criteria.push_back({
            "Altitude stable — no upset from spike (±300ft of 10000ft)",
            [](const FlightState& s) {
                return std::abs(s.altitude_ft - 10000.0) <= 300.0;
            }
        });
        suite.push_back(tc);
    }

    // ── TC-F03: Heading dropout during turn ──────────────────────────────────
    // Heading sensor drops to 0° for 10 seconds mid-turn.
    // Truth: aircraft continues turning. Sensor: frozen at 0°.
    {
        TestCase tc;
        tc.id   = "TC-F03";
        tc.name = "Heading Sensor Dropout During Turn";
        tc.description =
            "Heading sensor drops to 0° for 10s mid-turn. "
            "Aircraft continues turning; true heading diverges from sensor.";
        tc.fault_description =
            "HeadingSensor DROPOUT fault: t=5s–15s. "
            "Sensor returns 0° (null) for the full fault window.";
        tc.scenario   = Scenario::TURN_LEFT;
        tc.duration_s = 30.0;
        tc.initial_state.heading_deg  = 90.0;
        tc.initial_state.altitude_ft  = 10000.0;
        tc.initial_state.airspeed_kts = 250.0;

        tc.sensors.make_hdg = []() {
            return std::make_unique<FaultInjector>(
                std::make_unique<HeadingSensor>(),
                FaultInjector::FaultType::DROPOUT,
                5.0, 15.0
            );
        };

        tc.invariants.push_back({
            "Altitude must not deviate more than ±500ft during fault",
            [](const FlightState& s) {
                return std::abs(s.altitude_ft - 10000.0) <= 500.0;
            }
        });
        tc.criteria.push_back({
            "True heading turned left ≥20° despite sensor dropout",
            [](const FlightState& s) {
                // Started at 090; left turn; allow for wrap
                return s.heading_deg <= 72.0 || s.heading_deg >= 300.0;
            }
        });
        suite.push_back(tc);
    }

    // ── TC-F04: Altimeter drift (barometric pressure error) ──────────────────
    // Altimeter drifts +800ft over 30s, simulating wrong baro setting.
    // Truth: aircraft holds 35000ft. Sensor reads up to 35800ft.
    {
        TestCase tc;
        tc.id   = "TC-F04";
        tc.name = "Altimeter Drift — Baro Setting Error";
        tc.description =
            "Altimeter accumulates +800ft error over 30s simulating an "
            "incorrect barometric pressure setting at cruise altitude.";
        tc.fault_description =
            "AltitudeSensor DRIFT fault: t=10s–40s, magnitude=+800ft. "
            "Reported altitude grows linearly above truth during fault window.";
        tc.scenario   = Scenario::STRAIGHT_AND_LEVEL;
        tc.duration_s = 60.0;
        tc.initial_state.altitude_ft  = 35000.0;
        tc.initial_state.airspeed_kts = 280.0;
        tc.initial_state.heading_deg  = 0.0;

        tc.sensors.make_alt = []() {
            return std::make_unique<FaultInjector>(
                std::make_unique<AltitudeSensor>(),
                FaultInjector::FaultType::DRIFT,
                10.0, 40.0,
                800.0  // max drift of +800ft by end of window
            );
        };

        tc.invariants.push_back({
            "True altitude must remain within ±200ft of 35000ft",
            [](const FlightState& s) {
                return std::abs(s.altitude_ft - 35000.0) <= 200.0;
            }
        });
        tc.criteria.push_back({
            "True altitude stable at cruise (within ±100ft of 35000ft at end)",
            [](const FlightState& s) {
                return std::abs(s.altitude_ft - 35000.0) <= 100.0;
            }
        });
        suite.push_back(tc);
    }

    return suite;
}
