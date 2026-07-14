// competitive-verifier: PROBLEM https://www.spoj.com/problems/NKLEAVES/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_leftward_leaf_gathering_cost.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n,k; if(!(std::cin>>n>>k)) return 0; std::vector<int> weights(n);
    for(int& x:weights) std::cin>>x;
    std::cout<<minimum_leftward_leaf_gathering_cost(weights,k)<<'\n';
}
