// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc317/tasks/abc317_g

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/bipartite_edge_coloring.hpp"

int main(){
    int n, m;
    std::cin >> n >> m;
    std::vector<int> value(static_cast<std::size_t>(n * m));
    std::vector<BipartiteEdgeColoringEdge> edges;
    edges.reserve(static_cast<std::size_t>(n * m));
    for(int row = 0; row < n; ++row){
        for(int column = 0; column < m; ++column){
            const int id = row * m + column;
            std::cin >> value[static_cast<std::size_t>(id)];
            --value[static_cast<std::size_t>(id)];
            edges.push_back({row, value[static_cast<std::size_t>(id)]});
        }
    }
    const auto coloring = bipartite_edge_coloring(n, n, edges);
    std::vector<std::vector<int>> answer(
        static_cast<std::size_t>(n),
        std::vector<int>(static_cast<std::size_t>(m))
    );
    for(int id = 0; id < n * m; ++id){
        const int row = id / m;
        answer[static_cast<std::size_t>(row)][static_cast<std::size_t>(
            coloring.color[static_cast<std::size_t>(id)]
        )] = value[static_cast<std::size_t>(id)] + 1;
    }
    std::cout << "Yes\n";
    for(const auto& row: answer){
        for(int column = 0; column < m; ++column){
            if(column != 0) std::cout << ' ';
            std::cout << row[static_cast<std::size_t>(column)];
        }
        std::cout << '\n';
    }
}
