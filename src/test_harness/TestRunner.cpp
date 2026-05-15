#include "test_harness/TestRunner.h"
#include "simulation/SimEngine.h"
#include "sensors/AltitudeSensor.h"
#include "sensors/AirspeedSensor.h"
#include "sensors/HeadingSensor.h"
#include <memory>
#include <exception>
#include <sstream>
#include <filesystem>

TestRunner::TestRunner(std::string telemetry_dir)
    : telemetry_dir_(std::move(telemetry_dir)) {}

TestResult TestRunner::run(const TestCase& tc) {
    TestResult result;
    result.test_id   = tc.id;
    result.test_name = tc.name;
    result.duration_s = tc.duration_s;
    result.overall   = Verdict::PASS;
    result.invariant_violations = 0;

    try {
        SimEngine engine(0.1);
        engine.setState(tc.initial_state);

        auto alt_ptr = tc.sensors.make_alt
            ? tc.sensors.make_alt()
            : std::make_unique<AltitudeSensor>();
        auto spd_ptr = tc.sensors.make_spd
            ? tc.sensors.make_spd()
            : std::make_unique<AirspeedSensor>();
        auto hdg_ptr = tc.sensors.make_hdg
            ? tc.sensors.make_hdg()
            : std::make_unique<HeadingSensor>();

        Sensor& alt_sensor = *alt_ptr;
        Sensor& spd_sensor = *spd_ptr;
        Sensor& hdg_sensor = *hdg_ptr;

        TelemetryLogger logger;

        int steps = 0;
        engine.run(tc.duration_s, tc.scenario, [&](const FlightState& state) {
            ++steps;

            double s_alt = alt_sensor.read(state);
            double s_spd = spd_sensor.read(state);
            double s_hdg = hdg_sensor.read(state);
            logger.record(state, s_alt, s_spd, s_hdg);

            for (const auto& inv : tc.invariants) {
                if (!inv.evaluate(state)) {
                    ++result.invariant_violations;
                    CriterionResult cr;
                    cr.description = "[INVARIANT] " + inv.description;
                    cr.verdict     = Verdict::FAIL;
                    std::ostringstream oss;
                    oss << "Violated at t=" << state.time_s << "s";
                    cr.detail      = oss.str();
                    result.criterion_results.push_back(cr);
                }
            }
        });
        result.steps_run = steps;

        const FlightState& final_state = engine.getState();
        for (const auto& c : tc.criteria) {
            CriterionResult cr;
            cr.description = c.description;
            bool passed = c.evaluate(final_state);
            cr.verdict  = passed ? Verdict::PASS : Verdict::FAIL;
            result.criterion_results.push_back(cr);
            if (!passed) result.overall = Verdict::FAIL;
        }

        if (result.invariant_violations > 0) result.overall = Verdict::FAIL;

        if (!telemetry_dir_.empty()) {
            std::filesystem::create_directories(telemetry_dir_);
            logger.writeCSV(telemetry_dir_ + "/" + tc.id + ".csv");
        }

    } catch (const std::exception& e) {
        result.overall        = Verdict::ERROR;
        result.error_message  = e.what();
    }

    return result;
}

std::vector<TestResult> TestRunner::runAll(const std::vector<TestCase>& cases) {
    std::vector<TestResult> results;
    results.reserve(cases.size());
    for (const auto& tc : cases)
        results.push_back(run(tc));
    return results;
}
