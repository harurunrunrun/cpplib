// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/count_distinct_substrings.hpp"

int main(){
    assert(count_distinct_substrings(std::string()) == 0);
    assert(count_distinct_substrings(std::string("a")) == 1);
    assert(count_distinct_substrings(std::string("aaa")) == 3);
    assert(count_distinct_substrings(std::string("banana")) == 15);
    assert(count_distinct_substrings(std::vector<int>{1, 2, 1}) == 5);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string text;
        std::cin >> text;
        std::cout << count_distinct_substrings(text) << '\n';
    }
}
