// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/factorial
#include <algorithm>
#include <iostream>
#include <vector>
#include "../../src/algorithm/math/combinatorics/large_factorial_table.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int t;std::cin>>t;std::vector<unsigned int> queries(t);
    unsigned int maximum=0;
    for(auto& n:queries){std::cin>>n;maximum=std::max(maximum,n);}
    const LargeFactorialTable<998244353> table(maximum);
    for(auto n:queries)std::cout<<table.factorial(n)<<'\n';
}
