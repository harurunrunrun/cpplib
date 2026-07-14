// competitive-verifier: PROBLEM https://www.spoj.com/problems/MICEMAZE/

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/dijkstra.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int cell_count, exit_cell, time_limit, connection_count;
    if(!(std::cin >> cell_count >> exit_cell >> time_limit >> connection_count)){
        return 0;
    }

    std::vector<std::vector<DijkstraEdge<int>>> reversed_graph(
        static_cast<std::size_t>(cell_count)
    );
    for(int connection = 0; connection < connection_count; connection++){
        int source, destination, travel_time;
        std::cin >> source >> destination >> travel_time;
        reversed_graph[static_cast<std::size_t>(destination - 1)].push_back(
            {source - 1, travel_time}
        );
    }

    const auto shortest = dijkstra<int>(reversed_graph, exit_cell - 1);
    int escaped = 0;
    for(int cell = 0; cell < cell_count; cell++){
        if(shortest.reachable[static_cast<std::size_t>(cell)] &&
           shortest.dist[static_cast<std::size_t>(cell)] <= time_limit){
            escaped++;
        }
    }
    std::cout << escaped << '\n';
}
