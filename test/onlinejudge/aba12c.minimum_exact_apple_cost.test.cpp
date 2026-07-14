// competitive-verifier: PROBLEM https://www.spoj.com/problems/ABA12C/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_exact_apple_cost.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int n,k; std::cin>>n>>k; std::vector<int> prices(k);
        for(int& x:prices) std::cin>>x;
        std::cout<<minimum_exact_apple_cost(n,prices)<<'\n'; }
}
