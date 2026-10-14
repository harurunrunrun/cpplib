// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_cliques
#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/constraints/selection/weighted_clique_polynomial.hpp"
#include "../../src/structure/modint/modint.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n,m;std::cin>>n>>m;
    std::vector<Modint998244353> weights(n);
    for(auto& value:weights)std::cin>>value;
    std::vector<std::pair<int,int>> edges(m);
    for(auto& [u,v]:edges)std::cin>>u>>v;
    const auto coefficients=weighted_clique_polynomial(weights,edges);
    Modint998244353 answer=0;
    for(std::size_t i=1;i<coefficients.size();++i)answer+=coefficients[i];
    std::cout<<answer<<'\n';
}
