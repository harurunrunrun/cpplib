// competitive-verifier: PROBLEM https://www.spoj.com/problems/MIXTURES/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_mixture_smoke.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; while(std::cin>>n){ std::vector<int> colors(n); for(int& x:colors) std::cin>>x;
        std::cout<<minimum_mixture_smoke(colors)<<'\n'; }
}
