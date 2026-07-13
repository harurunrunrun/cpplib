// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/general_matching

#include <iostream>
#include "../../src/algorithm/matching/general_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    GeneralMatching matching(n);
    while(m--){
        int u, v;
        std::cin >> u >> v;
        matching.add_edge(u, v);
    }
    const auto result = matching.solve();
    std::cout << result.size << '\n';
    for(int v = 0; v < n; ++v){
        const int to = result.match[static_cast<std::size_t>(v)];
        if(v < to) std::cout << v << ' ' << to << '\n';
    }
}
