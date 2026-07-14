// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "../../src/algorithm/string/longest_common_substring_multiple_length.hpp"

void self_test(){
    assert(longest_common_substring_multiple_length<16>(
        std::vector<std::string>{"ababc", "babca", "zzabc"}
    ) == 3);
    assert(longest_common_substring_multiple_length<16>(
        std::vector<std::string>{"abc"}
    ) == 3);
    assert(longest_common_substring_multiple_length<16>(
        std::vector<std::string>{}
    ) == 0);
}

int main(){
    self_test();

    std::vector<std::string> texts;
    for(std::string text; std::cin >> text;) texts.push_back(std::move(text));
    if(texts.empty()) return 0;
    std::cout << longest_common_substring_multiple_length<100000>(texts)
              << '\n';
}
