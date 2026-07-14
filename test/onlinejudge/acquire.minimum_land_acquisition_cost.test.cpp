// competitive-verifier: PROBLEM https://www.spoj.com/problems/ACQUIRE/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_land_acquisition_cost.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; if(!(std::cin>>n)) return 0; std::vector<LandPlotDimensions> plots(n);
    for(auto& p:plots) std::cin>>p.width>>p.height;
    std::cout<<minimum_land_acquisition_cost(std::move(plots))<<'\n';
}
