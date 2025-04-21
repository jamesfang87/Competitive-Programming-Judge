#include "judge.hpp"
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std::filesystem;

// Helper functions
namespace {
std::vector<std::string> extract_output(const path& file_path) {
    std::ifstream file(file_path);
    std::vector<std::string> output;
    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty()) {
            output.push_back(line);
        }
    }
    return output;
}

std::vector<std::string> clean_output(const std::vector<std::string>& raw) {
    std::vector<std::string> cleaned;
    for (const auto& line : raw) {
        if (line.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
            cleaned.push_back(line);
        }
    }
    return cleaned;
}
} // namespace

// Judge implementation
Judge::Judge(int time_limit,
             int mem_limit,
             std::string compiler_flags,
             std::string test_data_path)
    : time_limit(time_limit),
      mem_limit(mem_limit),
      compiler_flags(std::move(compiler_flags)),
      test_data_path(std::move(test_data_path)) {}

int Judge::compile(std::string submission_path) {
    std::string cmd = "g++ " + compiler_flags + " " + submission_path + " -o " +
                      executable_path.string();
    return WEXITSTATUS(system(cmd.c_str()));
}

TestResult Judge::check_test(int exit_code, int test_num) {
    TestResult result;
    result.exit_code = exit_code;
    result.time_used = 0.0;
    result.mem_used = 0.0;
    if (exit_code != 0) {
        result.verdict = "RE";
        return result;
    }

    std::ifstream runtime_info("info.txt");
    std::string metrics;
    if (runtime_info >> result.time_used >> result.mem_used) {
        result.mem_used /= 1000.0; // Convert KB to MB
    }

    if (result.time_used > time_limit) {
        result.verdict = "TLE";
        return result;
    }

    if (result.mem_used > mem_limit) {
        result.verdict = "MLE";
        return result;
    }

    path expected_path = std::format("{}/{}.out", test_data_path, test_num);
    auto expected = clean_output(extract_output(expected_path));
    auto actual = clean_output(extract_output("out.txt"));

    result.verdict = (expected != actual) ? "WA" : "AC";
    return result;
}

TestResult Judge::run_test(int test_num) {
    std::string run_cmd = std::format("./{} < {}/{}.in > out.txt 2>&1",
                                      executable_path.string(),
                                      test_data_path,
                                      test_num);
    std::string cmd = "/usr/bin/time -f \"%e %M\" -o info.txt " + run_cmd;
    int ret = system(cmd.c_str());
    TestResult result = check_test(WEXITSTATUS(ret), test_num);
    std::string_view color = (result.verdict == "AC") ? "\x1b[32m" : "\x1b[31m";

    std::cout << std::format(
        "Test {}:\t{}{}\x1b[0m\tTime: {:.2f}ms\tMem: {:.1f}MB\n",
        test_num,
        color,
        result.verdict,
        result.time_used * 1000,
        result.mem_used);

    return result;
}

void Judge::run_tests(std::string submission_path) {
    std::cout << "Compiling...\n";
    if (compile(submission_path) != 0) {
        std::cerr << "\x1b[31mCompilation failed\x1b[0m\n";
        return;
    }
    std::cout << "\x1b[32mCompilation successful\n\x1b[0m";

    int test_count = 0;
    directory_iterator it(test_data_path), end;
    for (; it != end; ++it) {
        if (it->path().extension() == ".in") {
            test_count++;
        }
    }

    std::cout << std::format("\nRunning {} tests...\n", test_count);
    for (int i = 1; i <= test_count; ++i) {
        run_test(i);
    }

    // Cleanup
    remove("out.txt");
    remove(executable_path);
    remove("info.txt");
}
