// competitive-verifier: PROBLEM https://www.spoj.com/problems/APIO10A/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/maximum_commando_effectiveness.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int n; long long a,b,c; std::cin>>n>>a>>b>>c;
        std::vector<int> values(n); for(int& x:values) std::cin>>x;
        std::cout<<maximum_commando_effectiveness(values,a,b,c)<<'\n'; }
}
