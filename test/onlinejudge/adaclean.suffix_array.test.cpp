// competitive-verifier: PROBLEM https://www.spoj.com/problems/ADACLEAN/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/suffix_array.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        int size, length;
        std::string text;
        std::cin >> size >> length >> text;
        (void)size;
        std::cout << number_of_distinct_substrings_of_length(text, length) << '\n';
    }
}
