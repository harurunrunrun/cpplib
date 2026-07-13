// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/leiden.hpp"

namespace {

using approximate::graph::LeidenResult;
using approximate::graph::WeightedUndirectedEdge;

void require(bool condition){
    if(!condition) throw std::runtime_error("Leiden test failed");
}

bool close(long double left, long double right){
    const long double scale = std::max({1.0L, std::abs(left), std::abs(right)});
    return std::abs(left - right) <= 1.0e-11L * scale;
}

template<class Exception, class Function>
void require_throws(Function&& function){
    bool thrown = false;
    try{
        std::forward<Function>(function)();
    }catch(const Exception&){
        thrown = true;
    }
    require(thrown);
}

void check_connected(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& input_edges,
    const std::vector<std::size_t>& labels
){
    const auto graph = approximate::graph::detail::make_weighted_undirected_graph(
        vertex_count, input_edges
    );
    for(std::size_t start = 0; start < vertex_count; ++start){
        bool first = true;
        for(std::size_t previous = 0; previous < start; ++previous){
            if(labels[previous] == labels[start]) first = false;
        }
        if(!first) continue;
        std::vector<bool> visited(vertex_count, false);
        std::queue<std::size_t> queue;
        visited[start] = true;
        queue.push(start);
        while(!queue.empty()){
            const std::size_t vertex = queue.front();
            queue.pop();
            for(const auto& neighbor : graph.adjacency[vertex]){
                require(neighbor.weight > 0.0L);
                if(labels[neighbor.vertex] == labels[start] && !visited[neighbor.vertex]){
                    visited[neighbor.vertex] = true;
                    queue.push(neighbor.vertex);
                }
            }
        }
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            if(labels[vertex] == labels[start]) require(visited[vertex]);
        }
    }
}

void check_result(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    const LeidenResult& result,
    std::size_t maximum_levels,
    std::size_t maximum_passes
){
    require(result.labels.size() == vertex_count);
    require(result.levels <= maximum_levels);
    require(result.iterations <= maximum_levels * maximum_passes);
    std::vector<bool> seen(result.community_count, false);
    std::size_t next_label = 0;
    for(const std::size_t label : result.labels){
        require(label < result.community_count);
        if(!seen[label]){
            require(label == next_label);
            seen[label] = true;
            ++next_label;
        }
    }
    require(next_label == result.community_count);
    require(close(
        result.modularity,
        approximate::graph::community_modularity(vertex_count, edges, result.labels)
    ));
    std::vector<std::size_t> singleton(vertex_count);
    std::iota(singleton.begin(), singleton.end(), std::size_t{0});
    const long double baseline = approximate::graph::community_modularity(
        vertex_count, edges, singleton
    );
    require(result.modularity >= baseline || close(result.modularity, baseline));
    check_connected(vertex_count, edges, result.labels);
}

void boundary_tests(){
    using approximate::graph::leiden_communities;
    const long double infinity = std::numeric_limits<long double>::infinity();
    const long double nan = std::numeric_limits<long double>::quiet_NaN();
    const long double maximum = std::numeric_limits<long double>::max();
    require_throws<std::invalid_argument>([]{
        static_cast<void>(leiden_communities(2, {{0, 1, -1.0L}}, 0));
    });
    require_throws<std::invalid_argument>([infinity]{
        static_cast<void>(leiden_communities(2, {{0, 1, infinity}}, 0));
    });
    require_throws<std::invalid_argument>([nan]{
        static_cast<void>(leiden_communities(2, {{0, 1, nan}}, 0));
    });
    require_throws<std::out_of_range>([]{
        static_cast<void>(leiden_communities(2, {{0, 2, 1.0L}}, 0));
    });
    require_throws<std::overflow_error>([maximum]{
        static_cast<void>(leiden_communities(
            2, {{0, 1, maximum * 0.75L}, {1, 0, maximum * 0.75L}}, 0
        ));
    });
    const auto empty = leiden_communities(0, {}, 1);
    check_result(0, {}, empty, 20, 100);
    const auto no_edges = leiden_communities(3, {{0, 0, maximum}}, 1);
    require(no_edges.labels == std::vector<std::size_t>({0, 1, 2}));
    const auto no_levels = leiden_communities(2, {{0, 1, 1.0L}}, 1, 0, 10);
    require(no_levels.labels == std::vector<std::size_t>({0, 1}));
    const auto no_passes = leiden_communities(2, {{0, 1, 1.0L}}, 1, 10, 0);
    require(no_passes.labels == std::vector<std::size_t>({0, 1}));

    const auto weighted = approximate::graph::detail::make_weighted_undirected_graph(
        4, {{0, 1, 1.0L}, {2, 3, 1.0L}}
    );
    const auto level = approximate::graph::detail::make_louvain_level_graph(weighted);
    const auto refined = approximate::graph::detail::refine_connected_communities(
        level, {0, 0, 0, 0}
    );
    require(refined == std::vector<std::size_t>({0, 0, 1, 1}));

    const auto large = leiden_communities(2, {{0, 1, maximum / 4.0L}}, 9, 4, 8);
    check_result(2, {{0, 1, maximum / 4.0L}}, large, 4, 8);
}

}  // namespace

int main(){
    using approximate::graph::leiden_communities;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t case_count = 0;
    std::cin >> case_count;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t identifier = 0;
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::uint64_t seed = 0;
        std::size_t maximum_levels = 0;
        std::size_t maximum_passes = 0;
        std::cin >> identifier >> vertex_count >> edge_count >> seed >> maximum_levels
                 >> maximum_passes;
        std::vector<WeightedUndirectedEdge> edges(edge_count);
        for(auto& edge : edges) std::cin >> edge.left >> edge.right >> edge.weight;
        const auto result = leiden_communities(
            vertex_count, edges, seed, maximum_levels, maximum_passes
        );
        check_result(vertex_count, edges, result, maximum_levels, maximum_passes);
        const auto repeated = leiden_communities(
            vertex_count, edges, seed, maximum_levels, maximum_passes
        );
        require(result.labels == repeated.labels);
        require(result.iterations == repeated.iterations);
        std::cout << "CASE " << identifier << ' ' << result.community_count;
        for(const std::size_t label : result.labels) std::cout << ' ' << label;
        std::cout << '\n';
    }
}
