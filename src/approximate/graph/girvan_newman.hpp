#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_GIRVAN_NEWMAN_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_GIRVAN_NEWMAN_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "detail/undirected_graph.hpp"

namespace approximate::graph {

struct GirvanNewmanResult {
    std::vector<std::size_t> labels;
    std::size_t community_count = 0;
    std::vector<std::pair<std::size_t, std::size_t>> removed_edges;
    std::size_t rounds = 0;
};

namespace detail {

[[nodiscard]] inline std::pair<std::vector<std::size_t>, std::size_t> active_edge_components(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    const std::vector<bool>& active
){
    std::vector<std::vector<std::size_t>> adjacency(vertex_count);
    for(std::size_t edge = 0; edge < edges.size(); ++edge){
        if(!active[edge]) continue;
        adjacency[edges[edge].first].push_back(edges[edge].second);
        adjacency[edges[edge].second].push_back(edges[edge].first);
    }
    std::vector<std::size_t> labels(vertex_count, vertex_count);
    std::size_t community_count = 0;
    std::queue<std::size_t> queue;
    for(std::size_t start = 0; start < vertex_count; ++start){
        if(labels[start] != vertex_count) continue;
        labels[start] = community_count;
        queue.push(start);
        while(!queue.empty()){
            const std::size_t vertex = queue.front();
            queue.pop();
            for(const std::size_t neighbor : adjacency[vertex]){
                if(labels[neighbor] == vertex_count){
                    labels[neighbor] = community_count;
                    queue.push(neighbor);
                }
            }
        }
        ++community_count;
    }
    return {std::move(labels), community_count};
}

[[nodiscard]] inline std::vector<long double> unweighted_edge_betweenness(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    const std::vector<bool>& active
){
    std::vector<std::vector<std::pair<std::size_t, std::size_t>>> adjacency(vertex_count);
    for(std::size_t edge = 0; edge < edges.size(); ++edge){
        if(!active[edge]) continue;
        adjacency[edges[edge].first].emplace_back(edges[edge].second, edge);
        adjacency[edges[edge].second].emplace_back(edges[edge].first, edge);
    }
    std::vector<long double> betweenness(edges.size(), 0.0L);
    const std::size_t unreachable = std::numeric_limits<std::size_t>::max();
    for(std::size_t source = 0; source < vertex_count; ++source){
        std::vector<std::size_t> distance(vertex_count, unreachable);
        std::vector<long double> path_count(vertex_count, 0.0L);
        std::vector<std::vector<std::pair<std::size_t, std::size_t>>> predecessor(vertex_count);
        std::vector<std::size_t> order;
        order.reserve(vertex_count);
        std::queue<std::size_t> queue;
        distance[source] = 0;
        path_count[source] = 1.0L;
        queue.push(source);
        while(!queue.empty()){
            const std::size_t vertex = queue.front();
            queue.pop();
            order.push_back(vertex);
            for(const auto [neighbor, edge] : adjacency[vertex]){
                if(distance[neighbor] == unreachable){
                    distance[neighbor] = distance[vertex] + 1;
                    queue.push(neighbor);
                }
                if(distance[neighbor] == distance[vertex] + 1){
                    predecessor[neighbor].emplace_back(vertex, edge);
                    path_count[neighbor] += path_count[vertex];
                    if(!std::isfinite(path_count[neighbor])){
                        throw std::overflow_error("shortest-path count overflowed");
                    }
                }
            }
        }
        std::vector<long double> dependency(vertex_count, 0.0L);
        for(std::size_t index = order.size(); index > 0; --index){
            const std::size_t vertex = order[index - 1];
            for(const auto [previous, edge] : predecessor[vertex]){
                const long double contribution = (path_count[previous] / path_count[vertex])
                    * (1.0L + dependency[vertex]);
                if(!std::isfinite(contribution)){
                    throw std::overflow_error("edge betweenness overflowed");
                }
                betweenness[edge] += contribution;
                dependency[previous] += contribution;
                if(!std::isfinite(betweenness[edge]) || !std::isfinite(dependency[previous])){
                    throw std::overflow_error("edge betweenness overflowed");
                }
            }
        }
    }
    return betweenness;
}

}  // namespace detail

[[nodiscard]] inline GirvanNewmanResult girvan_newman(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& input_edges,
    std::size_t target_community_count,
    std::size_t maximum_rounds = std::numeric_limits<std::size_t>::max()
){
    if((vertex_count == 0 && target_community_count != 0)
       || (vertex_count != 0 && (target_community_count == 0 || target_community_count > vertex_count))){
        throw std::invalid_argument("target community count is outside the feasible range");
    }
    std::vector<std::pair<std::size_t, std::size_t>> edges;
    edges.reserve(input_edges.size());
    for(auto [left, right] : input_edges){
        detail::validate_endpoint(vertex_count, left);
        detail::validate_endpoint(vertex_count, right);
        if(left == right) continue;
        if(left > right) std::swap(left, right);
        edges.emplace_back(left, right);
    }
    std::sort(edges.begin(), edges.end());
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    std::vector<bool> active(edges.size(), true);
    GirvanNewmanResult result;
    auto [labels, community_count] = detail::active_edge_components(vertex_count, edges, active);
    if(community_count > target_community_count){
        throw std::invalid_argument(
            "target community count is smaller than the initial component count"
        );
    }
    while(community_count < target_community_count && result.rounds < maximum_rounds){
        const std::vector<long double> betweenness = detail::unweighted_edge_betweenness(
            vertex_count, edges, active
        );
        std::size_t best_edge = edges.size();
        long double best_value = -1.0L;
        for(std::size_t edge = 0; edge < edges.size(); ++edge){
            if(!active[edge]) continue;
            if(betweenness[edge] > best_value
               || (betweenness[edge] == best_value
                   && (best_edge == edges.size() || edges[edge] < edges[best_edge]))){
                best_value = betweenness[edge];
                best_edge = edge;
            }
        }
        if(best_edge == edges.size()) break;
        active[best_edge] = false;
        result.removed_edges.push_back(edges[best_edge]);
        ++result.rounds;
        std::tie(labels, community_count) = detail::active_edge_components(vertex_count, edges, active);
    }
    result.labels = std::move(labels);
    result.community_count = community_count;
    return result;
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_GIRVAN_NEWMAN_HPP_INCLUDED
