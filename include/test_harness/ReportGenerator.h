#pragma once
#include "test_harness/TestResult.h"
#include <vector>
#include <string>
#include <ostream>

class ReportGenerator {
public:
    // Print color-coded summary to stdout
    static void printConsole(const std::vector<TestResult>& results);

    // Write full report to a file
    static void writeFile(const std::vector<TestResult>& results,
                          const std::string& path);

private:
    static void writeReport(const std::vector<TestResult>& results,
                            std::ostream& out, bool use_color);
    static std::string verdictStr(Verdict v, bool color);
    static std::string timestamp();
};
