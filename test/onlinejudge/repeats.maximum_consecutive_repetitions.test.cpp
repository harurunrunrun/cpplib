// competitive-verifier: PROBLEM https://www.spoj.com/problems/REPEATS/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/sequence/maximum_consecutive_repetitions.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int length;
        std::cin >> length;
        std::string text(static_cast<std::size_t>(length), '\0');
        for(char& c: text) std::cin >> c;
        std::cout << maximum_consecutive_repetitions(text) << '\n';
    }
}
