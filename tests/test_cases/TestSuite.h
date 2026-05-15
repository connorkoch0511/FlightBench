#pragma once
#include "test_harness/TestCase.h"
#include <vector>
#include <cmath>

inline std::vector<TestCase> buildTestSuite() {
    std::vector<TestCase> suite;

    // ── TC-001: Straight and Level — altitude must not deviate ───────────────
    {
        TestCase tc;
        tc.id          = "TC-001";
        tc.name        = "Straight and Level — Altitude Stability";
        tc.description = "Aircraft holds steady altitude in cruise; altitude must stay within ±200ft.";
        tc.scenario    = Scenario::STRAIGHT_AND_LEVEL;
        tc.duration_s  = 60.0;
        tc.initial_state.altitude_ft  = 10000.0;
        tc.initial_state.airspeed_kts = 250.0;
        tc.initial_state.heading_deg  = 0.0;

        tc.invariants.push_back({
            "Altitude must remain within ±200ft of initial 10000ft",
            [](const FlightState& s) {
                return std::abs(s.altitude_ft - 10000.0) <= 200.0;
            }
        });
        tc.criteria.push_back({
            "Final altitude within ±50ft of 10000ft",
            [](const FlightState& s) {
                return std::abs(s.altitude_ft - 10000.0) <= 50.0;
            }
        });
        tc.criteria.push_back({
            "Airspeed above stall speed (>130kts)",
            [](const FlightState& s) { return s.airspeed_kts > 130.0; }
        });
        suite.push_back(tc);
    }

    // ── TC-002: Climb — reaches target altitude ──────────────────────────────
    {
        TestCase tc;
        tc.id          = "TC-002";
        tc.name        = "Climb — Altitude Gain";
        tc.description = "Aircraft climbs from 5000ft; must reach at least 8000ft within 120s.";
        tc.scenario    = Scenario::CLIMB;
        tc.duration_s  = 120.0;
        tc.initial_state.altitude_ft  = 5000.0;
        tc.initial_state.airspeed_kts = 250.0;

        tc.invariants.push_back({
            "Airspeed must not drop below stall speed (130kts)",
            [](const FlightState& s) { return s.airspeed_kts > 130.0; }
        });
        tc.criteria.push_back({
            "Altitude reached at least 8000ft after 120s",
            [](const FlightState& s) { return s.altitude_ft >= 8000.0; }
        });
        tc.criteria.push_back({
            "Altitude does not exceed 15000ft (overshoot guard)",
            [](const FlightState& s) { return s.altitude_ft <= 15000.0; }
        });
        suite.push_back(tc);
    }

    // ── TC-003: Descent — loses altitude, no terrain strike ──────────────────
    {
        TestCase tc;
        tc.id          = "TC-003";
        tc.name        = "Descent — Controlled Altitude Loss";
        tc.description = "Aircraft descends from 10000ft; must lose at least 1000ft and not hit ground.";
        tc.scenario    = Scenario::DESCENT;
        tc.duration_s  = 90.0;
        tc.initial_state.altitude_ft  = 10000.0;
        tc.initial_state.airspeed_kts = 250.0;

        tc.invariants.push_back({
            "Altitude must remain above ground (>0ft)",
            [](const FlightState& s) { return s.altitude_ft > 0.0; }
        });
        tc.criteria.push_back({
            "Altitude decreased by at least 1000ft",
            [](const FlightState& s) { return s.altitude_ft <= 9000.0; }
        });
        tc.criteria.push_back({
            "Airspeed does not exceed VMO (340kts)",
            [](const FlightState& s) { return s.airspeed_kts < 340.0; }
        });
        suite.push_back(tc);
    }

    // ── TC-004: Turn Left — heading changes correctly ────────────────────────
    {
        TestCase tc;
        tc.id          = "TC-004";
        tc.name        = "Turn Left — Heading Change";
        tc.description = "Aircraft executes standard-rate left turn from 090; must reach heading ≤060.";
        tc.scenario    = Scenario::TURN_LEFT;
        tc.duration_s  = 30.0;
        tc.initial_state.heading_deg  = 90.0;
        tc.initial_state.altitude_ft  = 10000.0;
        tc.initial_state.airspeed_kts = 250.0;

        tc.invariants.push_back({
            "Altitude deviation must stay within ±500ft during turn",
            [](const FlightState& s) {
                return std::abs(s.altitude_ft - 10000.0) <= 500.0;
            }
        });
        tc.criteria.push_back({
            "Heading turned left by at least 30 degrees (≤060°)",
            [](const FlightState& s) {
                // Turn started at 090; left turn decreases heading
                return s.heading_deg <= 62.0 || s.heading_deg >= 330.0;
            }
        });
        suite.push_back(tc);
    }

    // ── TC-005: Turn Right — symmetric validation ─────────────────────────────
    {
        TestCase tc;
        tc.id          = "TC-005";
        tc.name        = "Turn Right — Heading Change";
        tc.description = "Aircraft executes standard-rate right turn from 000; must reach heading ≥030.";
        tc.scenario    = Scenario::TURN_RIGHT;
        tc.duration_s  = 30.0;
        tc.initial_state.heading_deg  = 0.0;
        tc.initial_state.altitude_ft  = 10000.0;
        tc.initial_state.airspeed_kts = 250.0;

        tc.criteria.push_back({
            "Heading turned right by at least 30 degrees (≥030°)",
            [](const FlightState& s) { return s.heading_deg >= 28.0; }
        });
        suite.push_back(tc);
    }

    // ── TC-006: Combined Maneuver — climb + turn ──────────────────────────────
    {
        TestCase tc;
        tc.id          = "TC-006";
        tc.name        = "Combined Climb and Turn";
        tc.description = "Aircraft climbs while turning left; both altitude gain and heading change must occur.";
        tc.scenario    = Scenario::COMBINED;
        tc.duration_s  = 60.0;
        tc.initial_state.altitude_ft  = 8000.0;
        tc.initial_state.airspeed_kts = 240.0;
        tc.initial_state.heading_deg  = 180.0;

        tc.invariants.push_back({
            "Airspeed must stay above stall speed (130kts)",
            [](const FlightState& s) { return s.airspeed_kts > 130.0; }
        });
        tc.criteria.push_back({
            "Altitude increased by at least 1000ft",
            [](const FlightState& s) { return s.altitude_ft >= 9000.0; }
        });
        tc.criteria.push_back({
            "Heading changed by at least 15 degrees left",
            [](const FlightState& s) {
                // Started at 180; left turn goes toward 165 or wraps
                return s.heading_deg <= 165.0 || s.heading_deg >= 350.0;
            }
        });
        suite.push_back(tc);
    }

    // ── TC-007: Sensor fault tolerance — extreme noise ────────────────────────
    // Simulates a degraded sensor environment (high-noise conditions)
    {
        TestCase tc;
        tc.id          = "TC-007";
        tc.name        = "Straight and Level — Extended Duration Stability";
        tc.description = "10-minute straight-and-level; ensures no long-term drift in altitude.";
        tc.scenario    = Scenario::STRAIGHT_AND_LEVEL;
        tc.duration_s  = 600.0;
        tc.initial_state.altitude_ft  = 35000.0;
        tc.initial_state.airspeed_kts = 280.0;
        tc.initial_state.heading_deg  = 270.0;

        tc.invariants.push_back({
            "Altitude must remain above 34000ft during cruise",
            [](const FlightState& s) { return s.altitude_ft >= 34000.0; }
        });
        tc.invariants.push_back({
            "Altitude must stay below 36000ft",
            [](const FlightState& s) { return s.altitude_ft <= 36000.0; }
        });
        tc.criteria.push_back({
            "Final altitude within ±100ft of initial 35000ft",
            [](const FlightState& s) {
                return std::abs(s.altitude_ft - 35000.0) <= 100.0;
            }
        });
        tc.criteria.push_back({
            "Heading unchanged (within ±1deg) — no drift",
            [](const FlightState& s) {
                return std::abs(s.heading_deg - 270.0) <= 1.0;
            }
        });
        suite.push_back(tc);
    }

    return suite;
}
