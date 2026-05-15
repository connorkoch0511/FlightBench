#pragma once
#include "test_harness/TestCase.h"
#include "test_harness/TestResult.h"
#include "telemetry/TelemetryLogger.h"
#include <vector>
#include <string>

class TestRunner {
public:
    // If telemetry_dir is non-empty, a CSV is written per test case
    explicit TestRunner(std::string telemetry_dir = "");

    TestResult run(const TestCase& tc);
    std::vector<TestResult> runAll(const std::vector<TestCase>& cases);

private:
    std::string telemetry_dir_;
};
