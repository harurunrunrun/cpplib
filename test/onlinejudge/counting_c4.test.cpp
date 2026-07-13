// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/counting_c4

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/counting_c4.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [left, right]: edges) std::cin >> left >> right;
    const auto answer = count_c4_per_edge(n, edges);
    for(int edge = 0; edge < m; ++edge){
        if(edge != 0) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(edge)];
    }
    std::cout << '\n';
}
