// competitive-verifier: PROBLEM https://www.spoj.com/problems/FISHER/
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/minimum_cost_path_with_resource_limit.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, resource_limit;
    while(std::cin >> vertex_count >> resource_limit &&
          (vertex_count != 0 || resource_limit != 0)){
        std::vector<std::vector<int>> resource_matrix(
            static_cast<std::size_t>(vertex_count),
            std::vector<int>(static_cast<std::size_t>(vertex_count))
        );
        std::vector<std::vector<long long>> cost_matrix(
            static_cast<std::size_t>(vertex_count),
            std::vector<long long>(static_cast<std::size_t>(vertex_count))
        );
        for(auto& row: resource_matrix){
            for(int& value: row) std::cin >> value;
        }
        for(auto& row: cost_matrix){
            for(long long& value: row) std::cin >> value;
        }

        std::vector<std::vector<ResourceConstrainedEdge>> graph(
            static_cast<std::size_t>(vertex_count)
        );
        for(int from = 0; from < vertex_count; from++){
            for(int to = 0; to < vertex_count; to++){
                if(from == to) continue;
                graph[static_cast<std::size_t>(from)].push_back({
                    to,
                    resource_matrix[static_cast<std::size_t>(from)]
                                   [static_cast<std::size_t>(to)],
                    cost_matrix[static_cast<std::size_t>(from)]
                               [static_cast<std::size_t>(to)]
                });
            }
        }

        const auto answer = minimum_cost_path_with_resource_limit(
            graph,
            0,
            vertex_count - 1,
            resource_limit
        );
        if(!answer){
            std::cout << "-1 -1\n";
        }else{
            std::cout << answer->cost << ' ' << answer->resource_used << '\n';
        }
    }
    return 0;
}
