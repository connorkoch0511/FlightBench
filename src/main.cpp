#include "test_harness/TestRunner.h"
#include "test_harness/ReportGenerator.h"
#include "tests/test_cases/TestSuite.h"
#include "tests/test_cases/FaultTestSuite.h"
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
    std::string report_path   = "reports/test_report.txt";
    std::string telemetry_dir = "telemetry";

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: flightbench [--report PATH] [--telemetry DIR] [--no-telemetry]\n"
                      << "  --report PATH       Report file (default: reports/test_report.txt)\n"
                      << "  --telemetry DIR     Telemetry CSV directory (default: telemetry/)\n"
                      << "  --no-telemetry      Disable CSV export\n";
            return 0;
        } else if (arg == "--no-telemetry") {
            telemetry_dir.clear();
        } else if (arg == "--report" && i + 1 < argc) {
            report_path = argv[++i];
        } else if (arg == "--telemetry" && i + 1 < argc) {
            telemetry_dir = argv[++i];
        }
    }

    std::filesystem::create_directories(
        std::filesystem::path(report_path).parent_path()
    );

    std::cout << "\nFlightBench — Hardware-in-the-Loop Simulation Test Harness\n";
    std::cout << "Running test suite...\n\n";

    // Build combined suite: nominal + fault injection
    auto nominal = buildTestSuite();
    auto faults  = buildFaultTestSuite();

    std::vector<TestCase> all_cases;
    all_cases.insert(all_cases.end(), nominal.begin(), nominal.end());
    all_cases.insert(all_cases.end(), faults.begin(),  faults.end());

    TestRunner runner(telemetry_dir);
    auto results = runner.runAll(all_cases);

    ReportGenerator::printConsole(results);
    ReportGenerator::writeFile(results, report_path);

    int failures = 0;
    for (const auto& r : results)
        if (!r.passed()) ++failures;

    return failures > 0 ? 1 : 0;
}
