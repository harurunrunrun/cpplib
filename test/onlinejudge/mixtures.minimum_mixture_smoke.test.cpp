// competitive-verifier: PROBLEM https://www.spoj.com/problems/MIXTURES/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_adjacent_modular_merge_cost.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; while(std::cin>>n){
        std::vector<long long> values(n);
        for(long long& value:values) std::cin>>value;
        std::cout<<minimum_adjacent_modular_merge_cost(values,100)<<'\n';
    }
}
