#include "judge.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    // Default configuration
    int time_limit = 2;  // 2 seconds
    int mem_limit = 256; // 256MB
    std::string test_cases_dir = "test_cases";
    std::string submission_file = "test.cpp";
    std::string compiler_flags = " -std=c++17 -O2 -Wall -Wextra ";

    try {
        // Parse command-line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-t" || arg == "--time-limit") {
                if (++i >= argc)
                    throw std::runtime_error("Missing value for " + arg);
                time_limit = std::stoi(arg);
            } else if (arg == "-m" || arg == "--memory-limit") {
                if (++i >= argc)
                    throw std::runtime_error("Missing value for " + arg);
                mem_limit = std::stoi(arg);
            } else if (arg == "-d" || arg == "--test-dir") {
                if (++i >= argc)
                    throw std::runtime_error("Missing value for " + arg);
                test_cases_dir = arg;
            } else if (arg == "-s" || arg == "--submission") {
                if (++i >= argc)
                    throw std::runtime_error("Missing value for " + arg);
                submission_file = arg;
            } else if (arg == "-c" || arg == "--compiler-flags") {
                if (++i >= argc)
                    throw std::runtime_error("Missing value for " + arg);
                compiler_flags = " " + arg + " ";
            } else {
                throw std::runtime_error("Unknown option: " + arg);
            }
        }

        // Validate paths
        if (!std::filesystem::exists(test_cases_dir)) {
            throw std::runtime_error("Test directory not found: " +
                                     test_cases_dir);
        }
        if (!std::filesystem::exists(submission_file)) {
            throw std::runtime_error("Submission file not found: " +
                                     submission_file);
        }

        // Create and run judge
        Judge judge(time_limit, mem_limit, compiler_flags, test_cases_dir);
        judge.run_tests(submission_file);
    } catch (const std::exception& e) {
        std::cerr << "\x1b[31mError\x1b[0m: " << e.what() << "\n";
        std::cerr
            << "Usage: " << argv[0] << " [options]\n"
            << "Options:\n"
            << "  -t, --time-limit <sec>      Execution time limit (default: "
               "2)\n"
            << "  -m, --memory-limit <mb>     Memory limit (default: 256)\n"
            << "  -d, --test-dir <path>       Test cases directory (default: "
               "test_cases)\n"
            << "  -s, --submission <file>     Submission file (default: "
               "submission.cpp)\n"
            << "  -c, --compiler-flags <flags> Compiler flags (default: see "
               "below)\n"
            << "Default compiler flags: \"" << compiler_flags << "\"\n";
        return 1;
    }

    return 0;
}
