// competitive-verifier: PROBLEM https://www.spoj.com/problems/MSTRING/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/shortest_uncommon_subsequence.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string voki, toki;
    if(!(std::cin >> voki >> toki)) return 0;
    std::cout << shortest_uncommon_subsequence_length(voki, toki) << '\n';
}
