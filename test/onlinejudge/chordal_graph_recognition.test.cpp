// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/chordal_graph_recognition

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/chordal_graph_recognition.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [left, right]: edges) std::cin >> left >> right;
    const auto result = chordal_graph_recognition(n, edges);
    if(result.chordal){
        std::cout << "YES\n";
        for(int index = 0; index < n; ++index){
            if(index != 0) std::cout << ' ';
            std::cout << result.perfect_elimination_order[
                static_cast<std::size_t>(index)
            ];
        }
        std::cout << '\n';
    }else{
        std::cout << "NO\n" << result.induced_cycle.size() << '\n';
        for(std::size_t index = 0; index < result.induced_cycle.size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << result.induced_cycle[index];
        }
        std::cout << '\n';
    }
}
