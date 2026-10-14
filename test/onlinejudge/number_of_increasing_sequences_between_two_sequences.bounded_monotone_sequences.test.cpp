// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/number_of_increasing_sequences_between_two_sequences

#include <iostream>
#include <vector>
#include "../../src/algorithm/math/combinatorics/bounded_monotone_sequences.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n,m;std::cin>>n>>m;
    std::vector<int>a(n),b(n);for(int&x:a)std::cin>>x;for(int&x:b)std::cin>>x;
    std::cout<<count_bounded_monotone_sequences(a,b)<<'\n';
}
