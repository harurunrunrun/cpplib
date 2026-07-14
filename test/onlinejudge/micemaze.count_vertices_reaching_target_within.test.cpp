// competitive-verifier: PROBLEM https://www.spoj.com/problems/MICEMAZE/

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/count_vertices_reaching_target_within.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int cell_count, exit_cell, time_limit, connection_count;
    if(!(std::cin >> cell_count >> exit_cell >> time_limit >> connection_count)){
        return 0;
    }

    std::vector<ReachingTargetEdge<int>> edges;
    edges.reserve(static_cast<std::size_t>(connection_count));
    for(int connection = 0; connection < connection_count; connection++){
        int source, destination, travel_time;
        std::cin >> source >> destination >> travel_time;
        edges.push_back({source - 1, destination - 1, travel_time});
    }

    std::cout << count_vertices_reaching_target_within(
                     cell_count, edges, exit_cell - 1, time_limit
                 )
              << '\n';
}
