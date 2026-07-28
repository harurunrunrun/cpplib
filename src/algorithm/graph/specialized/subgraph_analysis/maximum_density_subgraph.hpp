#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_SUBGRAPH_ANALYSIS_MAXIMUM_DENSITY_SUBGRAPH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_SUBGRAPH_ANALYSIS_MAXIMUM_DENSITY_SUBGRAPH_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../../matching/flow/dinic.hpp"

struct MaximumDensitySubgraphResult{
    long long numerator;
    int denominator;
    long double density;
    std::vector<int> vertices;
};

inline std::optional<MaximumDensitySubgraphResult>
maximum_density_subgraph(
    int vertex_count,
    std::vector<std::pair<int, int>> edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(maximum_density_subgraph)."
        );
    }
    for(auto& [left, right]: edges){
        if(left < 0 || vertex_count <= left
            || right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(maximum_density_subgraph)."
            );
        }
        if(right < left) std::swap(left, right);
    }
    edges.erase(
        std::remove_if(edges.begin(), edges.end(), [](auto edge){
            return edge.first == edge.second;
        }),
        edges.end()
    );
    std::sort(edges.begin(), edges.end());
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    if(vertex_count == 0) return std::nullopt;
    if(edges.empty()){
        return MaximumDensitySubgraphResult{0, 1, 0.0L, {0}};
    }
    using Wide = __int128;
    const int edge_count = static_cast<int>(edges.size());
    long long numerator = 0;
    int denominator = 1;
    std::vector<int> best_vertices{0};
    while(true){
        const int source = 0;
        const int edge_offset = 1;
        const int vertex_offset = edge_offset + edge_count;
        const int sink = vertex_offset + vertex_count;
        Dinic<Wide> flow(sink + 1);
        const Wide total_edge_profit =
            static_cast<Wide>(denominator) * edge_count;
        const Wide infinity = total_edge_profit + 1;
        for(int edge_id = 0; edge_id < edge_count; ++edge_id){
            int node = edge_offset + edge_id;
            flow.add_edge(source, node, denominator);
            auto [left, right] = edges[static_cast<std::size_t>(edge_id)];
            flow.add_edge(node, vertex_offset + left, infinity);
            flow.add_edge(node, vertex_offset + right, infinity);
        }
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            flow.add_edge(
                vertex_offset + vertex, sink,
                static_cast<Wide>(numerator)
            );
        }
        flow.max_flow(source, sink, total_edge_profit);
        std::vector<char> source_side = flow.min_cut(source);
        std::vector<unsigned char> selected(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::vector<int> vertices;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(source_side[static_cast<std::size_t>(
                vertex_offset + vertex
            )] != 0){
                selected[static_cast<std::size_t>(vertex)] = 1;
                vertices.push_back(vertex);
            }
        }
        long long internal_edge_count = 0;
        for(auto [left, right]: edges){
            if(selected[static_cast<std::size_t>(left)] != 0
                && selected[static_cast<std::size_t>(right)] != 0){
                ++internal_edge_count;
            }
        }
        Wide improvement =
            static_cast<Wide>(denominator) * internal_edge_count
            - static_cast<Wide>(numerator)
                * static_cast<int>(vertices.size());
        if(improvement <= 0 || vertices.empty()) break;
        best_vertices = std::move(vertices);
        numerator = internal_edge_count;
        denominator = static_cast<int>(best_vertices.size());
        long long divisor = std::gcd(
            numerator, static_cast<long long>(denominator)
        );
        numerator /= divisor;
        denominator /= static_cast<int>(divisor);
    }
    return MaximumDensitySubgraphResult{
        numerator,
        denominator,
        static_cast<long double>(numerator)
            / static_cast<long double>(denominator),
        std::move(best_vertices)
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_SUBGRAPH_ANALYSIS_MAXIMUM_DENSITY_SUBGRAPH_HPP_INCLUDED
