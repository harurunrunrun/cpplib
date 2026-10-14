// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sharp_p_subset_sum
#include <iostream>
#include <vector>
#include "../../src/algorithm/math/combinatorics/subset_sum_counts.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n,t;std::cin>>n>>t;std::vector<int>weights(n);for(auto& x:weights)std::cin>>x;
    const auto answer=subset_sum_counts(weights,t);
    for(int i=1;i<=t;++i)std::cout<<answer[i]<<(i==t?'\n':' ');
}
