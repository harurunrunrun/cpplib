// competitive-verifier: PROBLEM https://www.spoj.com/problems/BRKSTRNG/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_string_break_cost.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    long long n; int m; while(std::cin>>n>>m){ std::vector<long long> cuts(m); for(auto& x:cuts) std::cin>>x;
        std::cout<<minimum_string_break_cost(n,cuts)<<'\n'; }
}
