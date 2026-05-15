#include "test_harness/TestRunner.h"
#include "test_harness/ReportGenerator.h"
#include "tests/test_cases/TestSuite.h"
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
    std::string report_path = "reports/test_report.txt";

    if (argc > 1) {
        std::string arg(argv[1]);
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: flightbench [report_output_path]\n"
                      << "  Default report path: " << report_path << "\n";
            return 0;
        }
        report_path = arg;
    }

    std::filesystem::create_directories(
        std::filesystem::path(report_path).parent_path()
    );

    std::cout << "\nFlightBench — Hardware-in-the-Loop Simulation Test Harness\n";
    std::cout << "Running test suite...\n\n";

    auto suite   = buildTestSuite();
    TestRunner   runner;
    auto results = runner.runAll(suite);

    ReportGenerator::printConsole(results);
    ReportGenerator::writeFile(results, report_path);

    int failures = 0;
    for (const auto& r : results)
        if (!r.passed()) ++failures;

    return failures > 0 ? 1 : 0;
}
