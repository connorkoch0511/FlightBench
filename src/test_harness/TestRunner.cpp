#include "test_harness/TestRunner.h"
#include "simulation/SimEngine.h"
#include <exception>
#include <sstream>

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

        int steps = 0;
        engine.run(tc.duration_s, tc.scenario, [&](const FlightState& state) {
            ++steps;
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
