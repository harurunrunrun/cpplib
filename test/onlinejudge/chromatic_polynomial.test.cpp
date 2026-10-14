// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/chromatic_polynomial
#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/constraints/coloring/chromatic_polynomial.hpp"
#include "../../src/structure/modint/modint.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n,m;std::cin>>n>>m;
    std::vector<std::pair<int,int>> edges(m);
    for(auto& [u,v]:edges)std::cin>>u>>v;
    auto answer=chromatic_polynomial<Modint998244353>(n,edges);
    for(int i=0;i<=n;++i)std::cout<<answer[i]<<(i==n?'\n':' ');
}
