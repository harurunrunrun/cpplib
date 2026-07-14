// competitive-verifier: PROBLEM https://www.spoj.com/problems/BAT3/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/maximum_batman_cliff_count.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int n,m; std::cin>>n>>m; std::vector<int> heights(n);
        for(int& x:heights) std::cin>>x;
        std::cout<<maximum_batman_cliff_count(heights,m)<<'\n'; }
}
