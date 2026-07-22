// competitive-verifier: PROBLEM https://www.spoj.com/problems/SUFEQPRE/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/automata/borders.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    for(int test_case = 1; test_case <= test_count; test_case++){
        std::string text;
        std::cin >> text;
        std::cout << "Case " << test_case << ": "
                  << border_lengths(text).size() << '\n';
    }
}
