// competitive-verifier: PROBLEM https://www.spoj.com/problems/DISUBSTR/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/suffix/count_distinct_substrings.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string text;
        std::cin >> text;
        std::cout << count_distinct_substrings(text) << '\n';
    }
}
