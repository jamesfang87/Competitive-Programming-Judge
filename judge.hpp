#include <string>
class Judge {
public:
    Judge(int time_limit, int mem_limit, std::string compiler_flags);

    int compile(std::string submission_path);
    std::string run_test(int test_num);
    std::string check_test(int exit_code, int test_num);
    void run_tests(std::string submission_path, std::string tests_path);

private:
    int time_limit, mem_limit;
    std::string compiler_flags = " -std=c++17 -O2 -lm -Wall ";
};
