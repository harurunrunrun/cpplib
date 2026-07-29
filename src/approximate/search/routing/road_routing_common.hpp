#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_ROAD_ROUTING_COMMON_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_ROAD_ROUTING_COMMON_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace road_routing_internal{

template<class Cost>
using Graph = std::vector<std::vector<std::pair<int, Cost>>>;

template<class Cost>
constexpr Cost infinity() noexcept{
    return std::numeric_limits<Cost>::max();
}

template<class Cost>
Cost add(Cost lhs, Cost rhs) noexcept{
    const Cost inf = infinity<Cost>();
    if(lhs == inf || rhs == inf) return inf;
    if(rhs > inf - lhs) return inf;
    return lhs + rhs;
}

template<class Cost>
void validate_graph(const Graph<Cost>& graph){
    static_assert(
        std::is_arithmetic_v<Cost> && !std::is_same_v<Cost, bool>,
        "road-routing costs must be non-boolean arithmetic values"
    );
    const int n = static_cast<int>(graph.size());
    for(const auto& edges : graph){
        for(const auto& [to, cost] : edges){
            bool invalid_cost = false;
            if constexpr(std::is_signed_v<Cost>){
                invalid_cost = cost < Cost{};
            }
            if constexpr(std::is_floating_point_v<Cost>){
                invalid_cost = invalid_cost || !std::isfinite(cost);
            }
            if(to < 0 || n <= to || invalid_cost || cost == infinity<Cost>()){
                throw std::invalid_argument(
                    "road routing requires valid vertices and finite non-negative costs"
                );
            }
        }
    }
}

inline void validate_vertex(int vertex, int vertex_count){
    if(vertex < 0 || vertex_count <= vertex){
        throw std::out_of_range("road-routing vertex is out of range");
    }
}

template<class Cost>
Graph<Cost> reverse_graph(const Graph<Cost>& graph){
    Graph<Cost> reverse(graph.size());
    for(int from = 0; from < static_cast<int>(graph.size()); ++from){
        for(const auto& [to, cost] : graph[static_cast<std::size_t>(from)]){
            reverse[static_cast<std::size_t>(to)].emplace_back(from, cost);
        }
    }
    return reverse;
}

template<class Cost>
std::vector<Cost> dijkstra(const Graph<Cost>& graph, int source){
    const int n = static_cast<int>(graph.size());
    validate_vertex(source, n);
    const Cost inf = infinity<Cost>();
    std::vector<Cost> distance(static_cast<std::size_t>(n), inf);
    using Entry = std::pair<Cost, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
    distance[static_cast<std::size_t>(source)] = Cost{};
    queue.emplace(Cost{}, source);
    while(!queue.empty()){
        const auto [current_distance, vertex] = queue.top();
        queue.pop();
        if(distance[static_cast<std::size_t>(vertex)] != current_distance){
            continue;
        }
        for(const auto& [to, cost] : graph[static_cast<std::size_t>(vertex)]){
            const Cost candidate = add(current_distance, cost);
            if(!(candidate < distance[static_cast<std::size_t>(to)])) continue;
            distance[static_cast<std::size_t>(to)] = candidate;
            queue.emplace(candidate, to);
        }
    }
    return distance;
}

template<class Cost>
std::vector<std::vector<Cost>> all_pairs_distances(const Graph<Cost>& graph){
    std::vector<std::vector<Cost>> result;
    result.reserve(graph.size());
    for(int source = 0; source < static_cast<int>(graph.size()); ++source){
        result.push_back(dijkstra(graph, source));
    }
    return result;
}

template<class Cost>
std::optional<Cost> optional_distance(Cost distance){
    if(distance == infinity<Cost>()) return std::nullopt;
    return distance;
}

} // namespace road_routing_internal

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_ROAD_ROUTING_COMMON_HPP_INCLUDED
