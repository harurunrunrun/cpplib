#ifndef CPPLIB_TEST_SUPPORT_NATIVE_STANDALONE_CHECKER_HPP_INCLUDED
#define CPPLIB_TEST_SUPPORT_NATIVE_STANDALONE_CHECKER_HPP_INCLUDED

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace native_standalone_checker{

struct Options{
    std::filesystem::path test;
    std::filesystem::path case_directory;
    std::filesystem::path build_directory;
    std::string compiler = "g++";
    std::string compiler_flags = "-std=c++20 -O2 -Wall -Wextra";
};

inline std::string shell_quote(const std::string& value){
    std::string result = "'";
    for(char character: value){
        if(character == '\'') result += "'\\''";
        else result += character;
    }
    result += '\'';
    return result;
}

inline Options parse_options(int argc, char** argv){
    Options result;
    for(int index = 1; index < argc; ++index){
        const std::string option = argv[index];
        if(index + 1 == argc) throw std::invalid_argument("missing checker option value");
        const std::string value = argv[++index];
        if(option == "--test") result.test = value;
        else if(option == "--case-dir") result.case_directory = value;
        else if(option == "--build-dir") result.build_directory = value;
        else if(option == "--cxx") result.compiler = value;
        else if(option == "--cxxflags") result.compiler_flags = value;
        else throw std::invalid_argument("unknown checker option: " + option);
    }
    if(result.test.empty() || result.case_directory.empty() || result.build_directory.empty()){
        throw std::invalid_argument("checker requires --test, --case-dir and --build-dir");
    }
    return result;
}

inline std::string read_file(const std::filesystem::path& path){
    std::ifstream stream(path, std::ios::binary);
    if(!stream) throw std::runtime_error("could not read " + path.string());
    return std::string(
        std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>()
    );
}

inline int run(int argc, char** argv){
    const Options options = parse_options(argc, argv);
    std::filesystem::create_directories(options.build_directory);
    const std::filesystem::path executable = options.build_directory / "test.out";
    const std::string compile_command =
        shell_quote(options.compiler) + " " + options.compiler_flags + " -I . " +
        shell_quote(options.test.string()) + " -o " + shell_quote(executable.string());
    if(std::system(compile_command.c_str()) != 0){
        std::cerr << "native checker: test compilation failed\n";
        return 1;
    }

    std::vector<std::filesystem::path> inputs;
    for(const auto& entry: std::filesystem::directory_iterator(options.case_directory)){
        if(entry.is_regular_file() && entry.path().extension() == ".in"){
            inputs.push_back(entry.path());
        }
    }
    std::sort(inputs.begin(), inputs.end());
    int failure_count = 0;
    if(inputs.empty()){
        std::cerr << "native checker: no input files\n";
        ++failure_count;
    }

    for(const auto& input: inputs){
        std::filesystem::path expected = input;
        expected.replace_extension(".out");
        if(!std::filesystem::exists(expected)){
            std::cerr << "native checker: missing " << expected << '\n';
            ++failure_count;
            continue;
        }
        const std::filesystem::path actual =
            options.build_directory / (input.stem().string() + ".actual");
        const std::filesystem::path error =
            options.build_directory / (input.stem().string() + ".stderr");
        const std::string execute_command =
            shell_quote(executable.string()) + " < " + shell_quote(input.string()) +
            " > " + shell_quote(actual.string()) + " 2> " + shell_quote(error.string());
        if(std::system(execute_command.c_str()) != 0){
            std::cerr << "native checker: test failed for " << input << '\n';
            ++failure_count;
            continue;
        }
        if(read_file(actual) != read_file(expected)){
            std::cerr << "native checker: output mismatch for " << input << '\n';
            ++failure_count;
        }
    }

    for(const auto& entry: std::filesystem::directory_iterator(options.case_directory)){
        if(!entry.is_regular_file() || entry.path().extension() != ".out") continue;
        std::filesystem::path input = entry.path();
        input.replace_extension(".in");
        if(!std::filesystem::exists(input)){
            std::cerr << "native checker: orphan output " << entry.path() << '\n';
            ++failure_count;
        }
    }

    if(failure_count != 0){
        std::cerr << "native checker: " << failure_count << " failure(s)\n";
        return 1;
    }
    std::cerr << "native checker: PASS " << inputs.size() << " case(s)\n";
    return 0;
}

} // namespace native_standalone_checker

#endif  // CPPLIB_TEST_SUPPORT_NATIVE_STANDALONE_CHECKER_HPP_INCLUDED
