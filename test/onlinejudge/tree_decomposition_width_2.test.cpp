// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/tree_decomposition_width_2
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/specialized/structural_analysis/tree_decomposition_width_two.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    std::string p, tw; int n, m;
    std::cin >> p >> tw >> n >> m;
    std::vector<std::pair<int,int>> edges(m);
    for(auto& [u,v]: edges){ std::cin >> u >> v; --u; --v; }
    const auto result = tree_decomposition_width_two(n,edges);
    if(!result.valid){ std::cout << "-1\n"; return 0; }
    std::cout << "s td " << result.bags.size() << ' ' << result.width() << ' ' << n << '\n';
    for(std::size_t i=0;i<result.bags.size();++i){
        std::cout << "b " << i+1;
        for(int v:result.bags[i]) std::cout << ' ' << v+1;
        std::cout << '\n';
    }
    for(const auto& [u,v]:result.tree_edges) std::cout << u+1 << ' ' << v+1 << '\n';
}
