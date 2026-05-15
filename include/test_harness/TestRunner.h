#pragma once
#include "test_harness/TestCase.h"
#include "test_harness/TestResult.h"
#include <vector>

class TestRunner {
public:
    TestResult run(const TestCase& tc);
    std::vector<TestResult> runAll(const std::vector<TestCase>& cases);
};
