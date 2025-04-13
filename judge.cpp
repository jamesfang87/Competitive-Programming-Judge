#include "judge.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

/**
 *
 */
double get_execution_time(const std::string& line) {
    /*
     * returns execution time of submission program
     */

    // runtime comes after the first occurrence of ":"
    std::string time = line.substr(line.find(':') + 1);
    return std::stod(time);
}

double get_memory_usage(const std::string& line) {
    /*
     * returns memory usage of submission program
     */

    // max resident contains memory usage it occurs after "avgdata"
    std::string mem = line.substr(line.find("avgdata ") + 8);
    return std::stod(mem) / 1000;
}

std::vector<std::string> extract_output(const std::string& file_path) {
    /*
     * extracts output of a file into a vector
     * each element in the vector is a line in the file
     */
    std::ifstream file(file_path);
    std::vector<std::string> output;

    // get next line in file
    std::string line;
    while (std::getline(file, line)) {
        // append to vector
        output.push_back(line);
    }
    file.close();

    return output;
}

std::vector<std::string>
clean_output(const std::vector<std::string>& raw_output) {
    /*
     * removes all whitespace from the end of a vector representing output to a
     * file
     */
    std::vector<std::string> ret;
    for (const std::string& line : raw_output) {
        if (line.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
            ret.push_back(line);
        }
    }
    return ret;
}

/**
 * @brief
 *
 *
 * Return codes key:
 * AC: Accepted
 * TLE: Time Limit Exceeded
 * MLE: Memory Limit Exceeded
 * RE: Runtime Error
 * WA: Wwrong Answer
 */
std::string Judge::check_test(int exit_code, int test_num) {
    // first check the exit code of the program
    if (exit_code != 0) {
        return "RE";
    }

    std::ifstream runtime_info("runtimeinfo.txt");
    std::string line;
    getline(runtime_info, line);
    runtime_info.close();

    // check time limit
    double execution_time = get_execution_time(line);
    if (execution_time > time_limit) {
        std::cout << "\x1b[31m" << "TLE\n" << "\x1b[0m" << std::endl;
        return "TLE";
    }

    // check mem limit
    if (get_memory_usage(line) > this->mem_limit) {
        std::cout << "\x1b[31m" << "MLE\n" << "\x1b[0m" << std::endl;
        return "MLE";
    }

    /*
     * WA and AC checks:
     */
    // extract expected output into a vector
    std::string expected_out_filepath =
        test_data_path + "/" + std::to_string(test_num) + ".out";
    std::vector<std::string> expected = extract_output(expected_out_filepath);

    // extract submission output into a vector
    std::vector<std::string> out = extract_output("submission_out.txt");

    // check for wrong answer
    if (clean_output(expected) != clean_output(out)) {
        std::cout << "\x1b[31m" << "WA" << "\x1b[0m";
    } else {
        std::cout << "\x1b[92m" << "AC" << "\x1b[0m";
    }
    return (clean_output(expected) != clean_output(out)) ? "WA" : "AC";
    // output execution time and memory usage only if WA or AC
    std::cout << "  \t" << execution_time * 1000 << " ms "
              << round(memory_usage * 10) / 10 << " mb" << std::endl;
}

std::string Judge::run_test(int test_num) {
    // base cmd: "./{executable_name}", simply runs the program
    std::string cmd = "gtime -o runtimeinfo.txt ./submission";

    // redirect input and output:
    std::string input_name =
        test_data_path + "/" + std::to_string(test_num) + ".in";
    cmd += " < " + input_name;
    cmd += " > submission_out.txt"; // program output, execution time, memory
                                    // usage written here

    // run program
    int submission_ret_code = system(cmd.c_str());

    check(WEXITSTATUS(submission_ret_code), test_num);
}

int Judge::compile(std::string src_path) {
    std::string cmd = "g++" + this->compiler_flags + src_path;
    int exit_code = system(cmd.c_str());

    // Check compilation exit code
    if (WEXITSTATUS(exit_code) != 0) {
        return 1;
    } else {
        return 0;
    }
}

void Judge::run_tests(std::string submission_path, std::string tests_path) {
    // first compile the given program
    int success = this->compile(submission_path);
    if (!success) {
        std::cerr << "Compilation failed\n" << std::endl;
        return;
    }
    std::cout << "Compilation successful\n" << std::endl;

    // now, run tests
    auto tests_it = std::filesystem::directory_iterator(test_data_path);

    // division by 2 since there are two files per test n: n.in, n.out
    int n = std::count_if(begin(tests_it), end(tests_it),
                          [](auto& entry) { return entry.is_regular_file(); }) /
            2;

    // run against tests
    std::cout << "Results:" << std::endl;
    for (int i = 1; i <= n; i++) {
        // run executable with test i as input
        std::cout << "Verdict for test case " << i << ": ";
        run_test(i);
    }
}

int main() {

    // remove output files
    std::filesystem::remove("submission_out.txt");
    std::filesystem::remove("submission");
    std::filesystem::remove("runtimeinfo.txt");
}
