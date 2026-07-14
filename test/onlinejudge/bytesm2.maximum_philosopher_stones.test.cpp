// competitive-verifier: PROBLEM https://www.spoj.com/problems/BYTESM2/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/maximum_philosopher_stones.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int h,w; std::cin>>h>>w;
        std::vector<std::vector<int>> grid(h,std::vector<int>(w)); for(auto& row:grid) for(int& x:row) std::cin>>x;
        std::cout<<maximum_philosopher_stones(grid)<<'\n'; }
}
