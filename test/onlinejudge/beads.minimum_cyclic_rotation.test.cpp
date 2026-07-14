// competitive-verifier: PROBLEM https://www.spoj.com/problems/BEADS/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/minimum_cyclic_rotation.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::string necklace;
        std::cin >> necklace;
        std::cout << minimum_cyclic_rotation_index(necklace) + 1 << '\n';
    }
}
