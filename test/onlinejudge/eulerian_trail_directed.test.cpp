// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/eulerian_trail_directed

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/traversal/eulerian_trail.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count--){
        int n, m;
        std::cin >> n >> m;
        std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
        for(auto& [from, to]: edges) std::cin >> from >> to;
        auto trail = directed_eulerian_trail(n, edges);
        if(!trail){
            std::cout << "No\n";
            continue;
        }
        std::cout << "Yes\n";
        for(int i = 0; i <= m; ++i){
            if(i != 0) std::cout << ' ';
            std::cout << trail->vertices[static_cast<std::size_t>(i)];
        }
        std::cout << '\n';
        for(int i = 0; i < m; ++i){
            if(i != 0) std::cout << ' ';
            std::cout << trail->edges[static_cast<std::size_t>(i)];
        }
        std::cout << '\n';
    }
}
