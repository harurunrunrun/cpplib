// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_lis_query

#include <iostream>
#include <vector>
#include "../../src/structure/range_query/value_query/static_range_lis.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n,q; std::cin>>n>>q;std::vector<int> a(n);for(int& x:a)std::cin>>x;
    StaticRangeLIS<int,100'000> queries(a);
    while(q--){int l,r;std::cin>>l>>r;std::cout<<queries.query(l,r)<<'\n';}
}
