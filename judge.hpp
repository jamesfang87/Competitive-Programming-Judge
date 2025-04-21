#ifndef JUDGE_HPP
#define JUDGE_HPP

#include <filesystem>
#include <string>

struct TestResult {
    int exit_code;
    double time_used; // in seconds
    double mem_used;  // in MB
    std::string verdict;
};

class Judge {
public:
    Judge(int time_limit, int mem_limit, std::string compiler_flags,
          std::string test_data_path);

    int compile(std::string submission_path);
    TestResult run_test(int test_num);
    void run_tests(std::string submission_path);

private:
    TestResult check_test(int exit_code, int test_num);

    int time_limit;
    int mem_limit;
    std::string compiler_flags;
    std::string test_data_path;
    std::filesystem::path executable_path = "submission";
};

#endif // JUDGE_HPP
