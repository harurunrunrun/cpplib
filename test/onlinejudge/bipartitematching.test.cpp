// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/bipartitematching

#include <iostream>
#include "../../src/algorithm/matching/bipartite_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int left, right, m;
    std::cin >> left >> right >> m;
    BipartiteMatching matching(left, right);
    while(m--){
        int a, b;
        std::cin >> a >> b;
        matching.add_edge(a, b);
    }
    const auto result = matching.solve();
    std::cout << result.size << '\n';
    for(int a = 0; a < left; ++a){
        if(result.left_match[static_cast<std::size_t>(a)] != -1){
            std::cout << a << ' ' << result.left_match[static_cast<std::size_t>(a)] << '\n';
        }
    }
}
