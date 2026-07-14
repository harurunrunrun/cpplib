// competitive-verifier: PROBLEM https://www.spoj.com/problems/TRT/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/maximum_treat_revenue.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; if(!(std::cin>>n)) return 0; std::vector<int> values(n); for(int& x:values) std::cin>>x;
    std::cout<<maximum_treat_revenue(values)<<'\n';
}
