// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/dominating_set.hpp"
#include "../../src/approximate/graph/independent_set.hpp"
#include "../../src/approximate/graph/vertex_cover.hpp"
#include "../../src/approximate/graph/detail/undirected_graph.hpp"

namespace {

void print_vector(const std::vector<std::size_t>& values){
    std::cout << values.size();
    for(const std::size_t value : values) std::cout << ' ' << value;
}

bool is_vertex_cover(
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    const std::vector<std::size_t>& cover,
    std::size_t vertex_count
){
    std::vector<bool> used(vertex_count, false);
    for(const std::size_t vertex : cover) used[vertex] = true;
    return std::all_of(edges.begin(), edges.end(), [&](const auto& edge){
        return used[edge.first] || used[edge.second];
    });
}

}  // namespace

int main(){
    using namespace approximate::graph;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const auto normalized = detail::make_undirected_graph(
        3, {{0, 1}, {1, 0}, {2, 2}}
    );
    if(normalized.adjacency != std::vector<std::vector<std::size_t>>{
        {1}, {0}, {}
    }){
        return 2;
    }
    if(normalized.self_loop != std::vector<bool>{false, false, true}){
        return 2;
    }
    bool rejected = false;
    try{
        static_cast<void>(greedy_dominating_set(1, {{0, 1}}));
    }catch(const std::out_of_range&){
        rejected = true;
    }
    if(!rejected) return 2;

    std::size_t query_count = 0;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::cin >> vertex_count >> edge_count;
        std::vector<std::pair<std::size_t, std::size_t>> edges(edge_count);
        for(auto& [left, right] : edges) std::cin >> left >> right;
        const VertexCoverResult cover = two_approximate_vertex_cover(vertex_count, edges);
        const auto dominating = greedy_dominating_set(vertex_count, edges);
        const auto independent = maximal_independent_set(vertex_count, edges);
        const auto minimum_degree = greedy_minimum_degree_independent_set(vertex_count, edges);
        if(!is_vertex_cover(edges, cover.vertices, vertex_count)) return 3;
        print_vector(cover.vertices);
        std::cout << " | ";
        print_vector(cover.matching_edge_indices);
        std::cout << " | ";
        print_vector(dominating);
        std::cout << " | ";
        print_vector(independent);
        std::cout << " | ";
        print_vector(minimum_degree);
        std::cout << '\n';
    }
}
