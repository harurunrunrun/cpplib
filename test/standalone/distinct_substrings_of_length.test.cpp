// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/suffix/count_distinct_substrings_of_length.hpp"

int main(){
    assert(count_distinct_substrings_of_length(std::string(), 1) == 0);
    assert(count_distinct_substrings_of_length(std::string("aaa"), 1) == 1);
    assert(count_distinct_substrings_of_length(std::string("aaa"), 2) == 1);
    assert(count_distinct_substrings_of_length(std::string("abac"), 2) == 3);
    assert(count_distinct_substrings_of_length(std::string("abc"), 0) == 0);
    assert(count_distinct_substrings_of_length(std::string("abc"), 4) == 0);
    const std::string bytes("\xff\x80\xff\0", 4);
    assert(count_distinct_substrings_of_length(bytes, 1) == 3);
    assert(count_distinct_substrings_of_length(bytes, 2) == 3);
    assert(count_distinct_substrings_of_length(
        std::vector<int>{1, 2, 1, 2}, 2
    ) == 2);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        int length;
        std::string text;
        std::cin >> length >> text;
        std::cout << count_distinct_substrings_of_length(text, length) << '\n';
    }
}
