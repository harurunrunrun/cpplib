// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/shortest_path_with_mode_switches.hpp"

void self_test(){
    assert(shortest_path_with_mode_switches(1, {}, {}) == 0);
    assert(shortest_path_with_mode_switches(2, {{0, 1, 1}}, {}) == 1);
    assert(shortest_path_with_mode_switches(2, {{0, 1, 0}}, {}) == -1);
    assert(shortest_path_with_mode_switches(2, {{0, 1, 0}}, {0}) == 1);
    assert(
        shortest_path_with_mode_switches(
            4,
            {{0, 1, 0}, {1, 2, 1}, {2, 3, 1}},
            {1},
            0,
            3,
            0
        ) == 3
    );

    bool thrown = false;
    try{
        (void)shortest_path_with_mode_switches(2, {{0, 2, 1}}, {});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int vertex_count, edge_count, switch_count;
    int source, target, initial_mode;
    if(!(std::cin >> vertex_count >> edge_count >> switch_count
                  >> source >> target >> initial_mode)){
        return 0;
    }
    std::vector<ModeSwitchEdge> edges(static_cast<std::size_t>(edge_count));
    for(auto& edge: edges) std::cin >> edge.from >> edge.to >> edge.mode;
    std::vector<int> switch_vertices(static_cast<std::size_t>(switch_count));
    for(int& vertex: switch_vertices) std::cin >> vertex;

    std::cout << shortest_path_with_mode_switches(
        vertex_count,
        edges,
        switch_vertices,
        source,
        target,
        initial_mode
    ) << '\n';
}
