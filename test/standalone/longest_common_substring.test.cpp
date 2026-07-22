// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

#include "../../src/algorithm/string/longest_common_substring.hpp"

namespace{

std::string decode(std::string value){
    return value == "-" ? std::string{} : value;
}

bool valid_result(
    const std::string& first,
    const std::string& second,
    const LongestCommonSubstringResult& result,
    int expected_length
){
    if(result.length != expected_length || result.first_pos < 0 ||
       result.second_pos < 0) return false;
    if(static_cast<std::size_t>(result.first_pos + result.length) >
           first.size() ||
       static_cast<std::size_t>(result.second_pos + result.length) >
           second.size()) return false;
    return first.compare(
        static_cast<std::size_t>(result.first_pos),
        static_cast<std::size_t>(result.length),
        second,
        static_cast<std::size_t>(result.second_pos),
        static_cast<std::size_t>(result.length)
    ) == 0;
}

} // namespace

int main(){
    const std::string byte_first("\xff\0\x80\xff", 4);
    const std::string byte_second("\x7f\0\x80\xff\x01", 5);
    const auto byte_result = longest_common_substring(
        byte_first, byte_second
    );
    if(byte_result.first_pos != 1 || byte_result.second_pos != 1 ||
       byte_result.length != 3) return 1;

    int case_count;
    if(!(std::cin >> case_count) || case_count < 0) return 2;
    for(int index = 0; index < case_count; ++index){
        std::string first;
        std::string second;
        int expected_length;
        if(!(std::cin >> first >> second >> expected_length)) return 2;
        first = decode(std::move(first));
        second = decode(std::move(second));
        if(!valid_result(
            first, second,
            longest_common_substring(first, second),
            expected_length
        )) return 1;
    }
    std::cout << "OK\n";
}
