// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/undirected_cycle_certificate.hpp"

namespace{

bool valid_certificate(
    const CycleDetectionResult& cycle,
    const std::vector<std::pair<int, int>>& edges
){
    if(cycle.vertices.empty() || cycle.vertices.size() != cycle.edges.size()) return false;
    std::vector<char> used(edges.size(), 0);
    for(std::size_t index = 0; index < cycle.vertices.size(); ++index){
        const int edge_id = cycle.edges[index];
        if(edge_id < 0 || static_cast<std::size_t>(edge_id) >= edges.size() ||
           used[static_cast<std::size_t>(edge_id)]) return false;
        used[static_cast<std::size_t>(edge_id)] = 1;
        const int from = cycle.vertices[index];
        const int to = cycle.vertices[(index + 1) % cycle.vertices.size()];
        const auto [left, right] = edges[static_cast<std::size_t>(edge_id)];
        if(!((left == from && right == to) || (left == to && right == from))) return false;
    }
    return true;
}

void self_test(){
    assert(undirected_cycle_certificate(3, {{0, 1}, {1, 2}}).vertices.empty());

    const std::vector<std::pair<int, int>> parallel{{0, 1}, {0, 1}};
    assert(valid_certificate(undirected_cycle_certificate(2, parallel), parallel));

    const std::vector<std::pair<int, int>> self_loop{{0, 0}};
    assert(valid_certificate(undirected_cycle_certificate(1, self_loop), self_loop));

    bool thrown = false;
    try{
        (void)undirected_cycle_certificate(2, {{0, 2}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    self_test();

    int vertex_count, edge_count;
    if(!(std::cin >> vertex_count >> edge_count)) return 0;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [left, right]: edges) std::cin >> left >> right;
    const auto cycle = undirected_cycle_certificate(vertex_count, edges);
    if(cycle.vertices.empty()){
        std::cout << -1 << '\n';
        return 0;
    }
    std::cout << cycle.vertices.size() << '\n';
    for(int vertex: cycle.vertices) std::cout << vertex << '\n';
    for(int edge: cycle.edges) std::cout << edge << '\n';
}
