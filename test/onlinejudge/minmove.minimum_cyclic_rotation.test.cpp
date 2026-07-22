// competitive-verifier: PROBLEM https://www.spoj.com/problems/MINMOVE/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/sequence/minimum_cyclic_rotation.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if(!(std::cin >> text)) return 0;
    std::cout << minimum_cyclic_rotation_index(text) << '\n';
}
