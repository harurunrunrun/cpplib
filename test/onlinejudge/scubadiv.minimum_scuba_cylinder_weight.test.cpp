// competitive-verifier: PROBLEM https://www.spoj.com/problems/SCUBADIV/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_scuba_cylinder_weight.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int oxygen,nitrogen,n; std::cin>>oxygen>>nitrogen>>n;
        std::vector<ScubaCylinder> cylinders(n); for(auto& c:cylinders) std::cin>>c.oxygen>>c.nitrogen>>c.weight;
        std::cout<<minimum_scuba_cylinder_weight(oxygen,nitrogen,cylinders)<<'\n'; }
}
