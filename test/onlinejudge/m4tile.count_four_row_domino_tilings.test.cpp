// competitive-verifier: PROBLEM https://www.spoj.com/problems/M4TILE/
#include <iostream>
#include "../../src/algorithm/other/dynamic_programming/count_four_row_domino_tilings.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; for(int index=1;index<=tests;index++){ int width; std::cin>>width;
        std::cout<<index<<' '<<count_four_row_domino_tilings(width)<<'\n'; }
}
