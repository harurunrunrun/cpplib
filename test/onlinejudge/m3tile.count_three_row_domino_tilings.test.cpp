// competitive-verifier: PROBLEM https://www.spoj.com/problems/M3TILE/
#include <iostream>
#include "../../src/algorithm/other/dynamic_programming/count_three_row_domino_tilings.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int width; while(std::cin>>width && width!=-1) std::cout<<count_three_row_domino_tilings(width)<<'\n';
}
