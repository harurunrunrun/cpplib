// competitive-verifier: PROBLEM https://www.spoj.com/problems/SHPATH/

#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../src/algorithm/graph/dijkstra.hpp"

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
        std::unordered_map<std::string, int> city_index;
        city_index.reserve(static_cast<std::size_t>(city_count) * 2);
        for(int city = 0; city < city_count; city++){
            std::string name;
            int neighbor_count;
            std::cin >> name >> neighbor_count;
            city_index.emplace(std::move(name), city);
            auto& edges = graph[static_cast<std::size_t>(city)];
            edges.reserve(static_cast<std::size_t>(neighbor_count));
            for(int edge = 0; edge < neighbor_count; edge++){
                int destination, cost;
                std::cin >> destination >> cost;
                edges.push_back({destination - 1, cost});
            }
        }

        std::vector<std::vector<int>> cached_distance(
            static_cast<std::size_t>(city_count)
        );
        std::vector<char> cached(static_cast<std::size_t>(city_count), 0);
        int query_count;
        std::cin >> query_count;
        while(query_count--){
            std::string source_name, destination_name;
            std::cin >> source_name >> destination_name;
            const int source = city_index.at(source_name);
            const int destination = city_index.at(destination_name);
            if(!cached[static_cast<std::size_t>(source)]){
                cached_distance[static_cast<std::size_t>(source)] =
                    dijkstra<int>(graph, source).dist;
                cached[static_cast<std::size_t>(source)] = 1;
            }
            std::cout << cached_distance[static_cast<std::size_t>(source)]
                                        [static_cast<std::size_t>(destination)]
                      << '\n';
        }
    }
}
