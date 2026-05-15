#include "test_harness/ReportGenerator.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace {
    const char* RESET  = "\033[0m";
    const char* GREEN  = "\033[32m";
    const char* RED    = "\033[31m";
    const char* YELLOW = "\033[33m";
    const char* BOLD   = "\033[1m";
    const char* CYAN   = "\033[36m";
}

std::string ReportGenerator::timestamp() {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return buf;
}

std::string ReportGenerator::verdictStr(Verdict v, bool color) {
    switch (v) {
        case Verdict::PASS:
            return color ? std::string(GREEN) + "PASS" + RESET : "PASS";
        case Verdict::FAIL:
            return color ? std::string(RED)   + "FAIL" + RESET : "FAIL";
        case Verdict::ERROR:
            return color ? std::string(YELLOW) + "ERROR" + RESET : "ERROR";
    }
    return "UNKNOWN";
}

void ReportGenerator::writeReport(const std::vector<TestResult>& results,
                                   std::ostream& out, bool use_color) {
    auto bold  = [&](const std::string& s) { return use_color ? std::string(BOLD) + s + RESET : s; };
    auto cyan  = [&](const std::string& s) { return use_color ? std::string(CYAN) + s + RESET : s; };

    out << bold("╔══════════════════════════════════════════════════════════╗") << "\n";
    out << bold("║            FlightBench Test Report                      ║") << "\n";
    out << bold("╚══════════════════════════════════════════════════════════╝") << "\n";
    out << "  Generated : " << timestamp() << "\n";
    out << "  Test Count: " << results.size() << "\n\n";

    int passed = 0, failed = 0, errored = 0;
    for (const auto& r : results) {
        if (r.overall == Verdict::PASS)  ++passed;
        if (r.overall == Verdict::FAIL)  ++failed;
        if (r.overall == Verdict::ERROR) ++errored;
    }

    for (const auto& r : results) {
        out << cyan("──────────────────────────────────────────────────────────") << "\n";
        out << "  [" << verdictStr(r.overall, use_color) << "] "
            << bold(r.test_id + " — " + r.test_name) << "\n";
        out << "  Duration: " << std::fixed << std::setprecision(1)
            << r.duration_s << "s | Steps: " << r.steps_run;
        if (r.invariant_violations > 0)
            out << " | Invariant violations: " << r.invariant_violations;
        out << "\n";

        if (!r.error_message.empty())
            out << "  ERROR: " << r.error_message << "\n";

        for (const auto& cr : r.criterion_results) {
            out << "    " << verdictStr(cr.verdict, use_color)
                << "  " << cr.description;
            if (!cr.detail.empty()) out << "  [" << cr.detail << "]";
            out << "\n";
        }
        out << "\n";
    }

    out << cyan("══════════════════════════════════════════════════════════") << "\n";
    out << bold("  SUMMARY: ");
    out << verdictStr(Verdict::PASS,  use_color) << " " << passed  << "  ";
    out << verdictStr(Verdict::FAIL,  use_color) << " " << failed  << "  ";
    if (errored > 0)
        out << verdictStr(Verdict::ERROR, use_color) << " " << errored;
    out << "\n";
    out << "  Total: " << results.size() << " tests";
    if (failed == 0 && errored == 0)
        out << (use_color ? std::string("  ") + GREEN + "All tests passed." + RESET : "  All tests passed.");
    out << "\n";
    out << cyan("══════════════════════════════════════════════════════════") << "\n";
}

void ReportGenerator::printConsole(const std::vector<TestResult>& results) {
    writeReport(results, std::cout, true);
}

void ReportGenerator::writeFile(const std::vector<TestResult>& results,
                                 const std::string& path) {
    std::ofstream f(path);
    if (!f) {
        std::cerr << "Warning: could not open report file: " << path << "\n";
        return;
    }
    writeReport(results, f, false);
    std::cout << "Report written to: " << path << "\n";
}
