// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/traversal/cycle_detection.hpp"

bool valid_directed_cycle(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    const CycleDetectionResult& cycle
){
    if(cycle.vertices.size() != cycle.edges.size() || cycle.edges.empty()) return false;
    std::vector<char> used_vertex(static_cast<std::size_t>(n), 0);
    std::vector<char> used_edge(edges.size(), 0);
    for(std::size_t index = 0; index < cycle.edges.size(); ++index){
        const int vertex = cycle.vertices[index];
        const int next = cycle.vertices[(index + 1) % cycle.vertices.size()];
        const int edge = cycle.edges[index];
        if(vertex < 0 || n <= vertex || edge < 0 || edges.size() <= static_cast<std::size_t>(edge)) return false;
        if(used_vertex[static_cast<std::size_t>(vertex)] || used_edge[static_cast<std::size_t>(edge)]) return false;
        used_vertex[static_cast<std::size_t>(vertex)] = 1;
        used_edge[static_cast<std::size_t>(edge)] = 1;
        if(edges[static_cast<std::size_t>(edge)] != std::pair{vertex, next}) return false;
    }
    return true;
}

bool valid_undirected_cycle(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    const CycleDetectionResult& cycle
){
    if(cycle.vertices.size() != cycle.edges.size() || cycle.edges.empty()) return false;
    std::vector<char> used_vertex(static_cast<std::size_t>(n), 0);
    std::vector<char> used_edge(edges.size(), 0);
    for(std::size_t index = 0; index < cycle.edges.size(); ++index){
        const int vertex = cycle.vertices[index];
        const int next = cycle.vertices[(index + 1) % cycle.vertices.size()];
        const int edge = cycle.edges[index];
        if(vertex < 0 || n <= vertex || edge < 0 || edges.size() <= static_cast<std::size_t>(edge)) return false;
        if(used_vertex[static_cast<std::size_t>(vertex)] || used_edge[static_cast<std::size_t>(edge)]) return false;
        used_vertex[static_cast<std::size_t>(vertex)] = 1;
        used_edge[static_cast<std::size_t>(edge)] = 1;
        const auto [from, to] = edges[static_cast<std::size_t>(edge)];
        if(!((from == vertex && to == next) || (from == next && to == vertex))) return false;
    }
    return true;
}

void self_test(){
    {
        const std::vector<std::pair<int, int>> edges = {{0, 1}, {0, 1}, {1, 2}, {2, 0}};
        const auto cycle = directed_cycle_with_edges(3, edges);
        assert(valid_directed_cycle(3, edges, cycle));
    }
    {
        const std::vector<std::pair<int, int>> edges = {{0, 0}};
        const auto cycle = directed_cycle_with_edges(1, edges);
        assert(valid_directed_cycle(1, edges, cycle));
        assert(cycle.edges == std::vector<int>{0});
    }
    {
        const std::vector<std::pair<int, int>> edges = {{0, 1}, {1, 2}, {0, 2}};
        const auto cycle = directed_cycle_with_edges(3, edges);
        assert(cycle.vertices.empty());
        assert(cycle.edges.empty());
    }
    {
        const std::vector<std::pair<int, int>> edges = {{0, 1}, {0, 1}};
        const auto cycle = undirected_cycle_with_edges(2, edges);
        assert(valid_undirected_cycle(2, edges, cycle));
        assert(cycle.edges.size() == 2);
    }
    {
        const std::vector<std::pair<int, int>> edges = {{0, 0}};
        const auto cycle = undirected_cycle_with_edges(1, edges);
        assert(valid_undirected_cycle(1, edges, cycle));
    }
    {
        constexpr int n = 500000;
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(n));
        for(int vertex = 0; vertex + 1 < n; ++vertex) edges.push_back({vertex, vertex + 1});
        auto no_cycle = directed_cycle_with_edges(n, edges);
        assert(no_cycle.edges.empty());
        edges.push_back({n - 1, 0});
        auto cycle = directed_cycle_with_edges(n, edges);
        assert(valid_directed_cycle(n, edges, cycle));
        assert(cycle.edges.size() == static_cast<std::size_t>(n));
    }
    {
        bool thrown = false;
        try{
            (void)directed_cycle_with_edges(2, {{0, 2}});
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int index = 0; index < m; ++index){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    std::cout << static_cast<int>(has_directed_cycle(graph)) << '\n';
}
