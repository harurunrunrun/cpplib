// competitive-verifier: PROBLEM https://www.spoj.com/problems/ASSIGN/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/count_perfect_assignments.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int n; std::cin>>n; std::vector<std::vector<int>> preferences(n,std::vector<int>(n));
        for(auto& row:preferences) for(int& x:row) std::cin>>x;
        std::cout<<count_perfect_assignments(preferences)<<'\n'; }
}
