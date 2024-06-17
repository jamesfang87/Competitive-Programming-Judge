#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <cmath>
#include <sstream>

std::string src_path = "\"path/to/src\"";
std::string test_data_path = "path/to/testdata"; // make sure there are no spaces in the path
double time_limit = 2.0; // 2s = 2000ms
double memory_limit = 256; // 256mb
std::string compiler_flags = " -std=c++17 -O2 -lm -Wall "; // keep the leading and trailing spaces

double get_execution_time(const std::string& line) {
    // check time limit
    // runtime comes after the first occurrence of ":"
    std::string time = line.substr(line.find(':') + 1);
    return std::stod(time);
}

double get_memory_used(const std::string& line) {
    // check memory limit
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

std::vector<std::string> clean_output(const std::vector<std::string>& raw_output) {
    /*
     * removes all whitespace from the end of a vector representing output to a file
     */
    std::vector<std::string> ret;
    for (const std::string& line : raw_output) {
        if (line.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
            ret.push_back(line);
        }
    }
    return ret;
}


void check(int exit_code, int test_number) {
    /*
     * return codes:
     * AC: accepted solution
     * TLE: time limit exceeded
     * MLE: memory limit exceeded
     * RE: runtime error
     * WA: wrong answer
     */
    std::cout << "Verdict for test case " << test_number << ": ";

    /*
     * exit code check:
     */
    if (exit_code != 0) {
        std::cout << "\t  \x1b[31m" << "RE with exit code: " << exit_code << "\x1b[0m" << std::endl;
        return;
    }

    /*
     * time and memory limit checks:
     */

    // get the first line from runtimeinfo.txt
    // it contains information about execution time and memory usage
    std::ifstream runtime_info("runtimeinfo.txt");
    std::string line; getline(runtime_info, line);
    runtime_info.close();

    // check time limit
    double execution_time = get_execution_time(line);
    if (execution_time > time_limit) {
        std::cout << "\x1b[31m" << "TLE\n" << "\x1b[0m" << std::endl;
        return;
    }

    // check mem limit
    double memory_usage = get_memory_used(line);
    if (memory_usage > memory_limit) {
        std::cout << "\x1b[31m" << "MLE\n" << "\x1b[0m" << std::endl;
        return;
    }

    /*
     * WA and AC checks:
     */
    // extract expected output into a vector
    std::string expected_out_filepath = test_data_path + "/" + std::to_string(test_number) + ".out";
    std::vector<std::string> expected_out = extract_output(expected_out_filepath);

    // extract submission output into a vector
    std::vector<std::string> submission_out = extract_output("submission_out.txt");

    // check for wrong answer
    if (clean_output(expected_out) != clean_output(submission_out)) {
        std::cout << "\x1b[31m" << "WA" << "\x1b[0m";
    } else {
        std::cout << "\x1b[92m" << "AC" << "\x1b[0m";
    }

    // output execution time and memory usage only if WA or AC
    std::cout << "  \t" << execution_time * 1000 << " ms " << round(memory_usage * 10) / 10 << " mb" << std::endl;
}

void run(int test_number) {
    // base cmd: "./{executable_name}", simply runs the program
    std::string cmd = "gtime -o runtimeinfo.txt ./submission";

    // redirect input and output:
    std::string input_name = test_data_path + "/" + std::to_string(test_number) + ".in";
    cmd += " < " + input_name;
    cmd += " > submission_out.txt"; // program output, execution time, memory usage written here

    // run program
    int submission_ret_code = system(cmd.c_str());
    check(WEXITSTATUS(submission_ret_code), test_number);
}

int compile() {
    std::string compile_command = "g++" + compiler_flags + src_path + " -o submission";
    int exit_code = system(compile_command.c_str());

    // Check compilation exit code
    if (WEXITSTATUS(exit_code) != 0) {
        std::cerr << "Compilation failed\n" << std::endl;
        return 1;
    } else {
        std::cout << "Compilation successful\n" << std::endl;
        return 0;
    }
}

int main() {
    int compile_ret_code = compile();
    // exit program if submission does not compile
    if (compile_ret_code != 0) {
        return 0;
    }

    // iterator to the directory that the test data is stored in
    auto directory_it = std::filesystem::directory_iterator(test_data_path);

    // division by 2 since there are two files per test case n: n.in, n.out
    int num_test_cases = std::count_if(
            begin(directory_it),
            end(directory_it),
            [](auto& entry) { return entry.is_regular_file(); }
    ) / 2;

    // run against test cases
    std::cout << "Results are displayed below:" << std::endl;
    for (int i = 1; i <= num_test_cases; i++) {
        // run executable with test case i as input
        run(i);
    }

    // remove output files
    std::filesystem::remove("submission_out.txt");
    std::filesystem::remove("submission");
    std::filesystem::remove("runtimeinfo.txt");
}
