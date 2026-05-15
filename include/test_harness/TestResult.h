#pragma once
#include <string>
#include <vector>

enum class Verdict { PASS, FAIL, ERROR };

struct CriterionResult {
    std::string description;
    Verdict     verdict;
    std::string detail;
};

struct TestResult {
    std::string  test_id;
    std::string  test_name;
    Verdict      overall;
    double       duration_s;
    int          steps_run;
    int          invariant_violations;
    std::vector<CriterionResult> criterion_results;
    std::string  error_message;

    bool passed() const { return overall == Verdict::PASS; }
};
