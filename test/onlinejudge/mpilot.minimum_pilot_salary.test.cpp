// competitive-verifier: PROBLEM https://www.spoj.com/problems/MPILOT/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_pilot_salary.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; if(!(std::cin>>n)) return 0; std::vector<PilotSalaries> pilots(n);
    for(auto& p:pilots) std::cin>>p.captain>>p.assistant;
    std::cout<<minimum_pilot_salary(pilots)<<'\n';
}
