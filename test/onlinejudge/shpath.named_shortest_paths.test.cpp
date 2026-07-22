// competitive-verifier: PROBLEM https://www.spoj.com/problems/SHPATH/

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/named_shortest_paths.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int city_count;
        std::cin >> city_count;
        std::vector<std::vector<DijkstraEdge<int>>> graph(
            static_cast<std::size_t>(city_count)
        );
        std::vector<std::string> names(static_cast<std::size_t>(city_count));
        for(int city = 0; city < city_count; city++){
            int neighbor_count;
            std::cin >> names[static_cast<std::size_t>(city)] >> neighbor_count;
            auto& edges = graph[static_cast<std::size_t>(city)];
            edges.reserve(static_cast<std::size_t>(neighbor_count));
            for(int edge = 0; edge < neighbor_count; edge++){
                int destination, cost;
                std::cin >> destination >> cost;
                edges.push_back({destination - 1, cost});
            }
        }

        NamedShortestPaths<int> shortest_paths(
            std::move(names), std::move(graph)
        );
        int query_count;
        std::cin >> query_count;
        while(query_count--){
            std::string source_name, destination_name;
            std::cin >> source_name >> destination_name;
            std::cout << shortest_paths.distance(source_name, destination_name)
                             .value_or(-1)
                      << '\n';
        }
    }
}
