#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_ARC_FLAGS_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_ARC_FLAGS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "road_routing_common.hpp"

template<class Cost>
class ArcFlagsRouting{
public:
    using cost_type = Cost;
    using graph_type = road_routing_internal::Graph<Cost>;

    ArcFlagsRouting(
        graph_type graph,
        std::vector<int> region_of_vertex,
        int region_count = -1
    )
        : region_of_vertex_(std::move(region_of_vertex)){
        road_routing_internal::validate_graph(graph);
        const int n = static_cast<int>(graph.size());
        if(static_cast<int>(region_of_vertex_.size()) != n){
            throw std::invalid_argument(
                "ArcFlagsRouting needs one region for every vertex"
            );
        }
        int inferred_region_count = 0;
        for(const int region : region_of_vertex_){
            if(region < 0){
                throw std::invalid_argument(
                    "ArcFlagsRouting region identifiers must be non-negative"
                );
            }
            inferred_region_count = std::max(inferred_region_count, region + 1);
        }
        if(region_count < 0) region_count = inferred_region_count;
        if(region_count < inferred_region_count || (n != 0 && region_count == 0)){
            throw std::invalid_argument(
                "ArcFlagsRouting region_count does not cover all regions"
            );
        }
        region_count_ = region_count;
        const std::size_t word_count =
            (static_cast<std::size_t>(region_count_) + 63U) / 64U;
        graph_.resize(graph.size());
        for(int from = 0; from < n; ++from){
            auto& destination = graph_[static_cast<std::size_t>(from)];
            destination.reserve(graph[static_cast<std::size_t>(from)].size());
            for(const auto& [to, cost] : graph[static_cast<std::size_t>(from)]){
                destination.push_back(FlaggedArc{
                    to, cost, std::vector<std::uint64_t>(word_count, 0)
                });
            }
        }
        preprocess_flags(graph);
    }

    [[nodiscard]] int vertex_count() const noexcept{
        return static_cast<int>(graph_.size());
    }

    [[nodiscard]] int region_count() const noexcept{
        return region_count_;
    }

    [[nodiscard]] int region_of(int vertex) const{
        road_routing_internal::validate_vertex(vertex, vertex_count());
        return region_of_vertex_[static_cast<std::size_t>(vertex)];
    }

    [[nodiscard]] bool arc_allows_region(
        int from,
        std::size_t edge_index,
        int region
    ) const{
        road_routing_internal::validate_vertex(from, vertex_count());
        if(region < 0 || region_count_ <= region){
            throw std::out_of_range("ArcFlagsRouting region is out of range");
        }
        const auto& arc = graph_[static_cast<std::size_t>(from)].at(edge_index);
        return (
            arc.flags[static_cast<std::size_t>(region) / 64U]
            >> (static_cast<unsigned>(region) & 63U)
        ) & 1U;
    }

    [[nodiscard]] std::optional<Cost> query_distance(
        int source,
        int target
    ) const{
        const int n = vertex_count();
        road_routing_internal::validate_vertex(source, n);
        road_routing_internal::validate_vertex(target, n);
        if(source == target) return Cost{};
        const int target_region =
            region_of_vertex_[static_cast<std::size_t>(target)];
        const Cost inf = road_routing_internal::infinity<Cost>();
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
            if(vertex == target) return current_distance;
            const auto& edges = graph_[static_cast<std::size_t>(vertex)];
            for(std::size_t edge_index = 0; edge_index < edges.size(); ++edge_index){
                if(!arc_allows_region(vertex, edge_index, target_region)) continue;
                const auto& arc = edges[edge_index];
                const Cost candidate =
                    road_routing_internal::add(current_distance, arc.cost);
                if(!(candidate < distance[static_cast<std::size_t>(arc.to)])){
                    continue;
                }
                distance[static_cast<std::size_t>(arc.to)] = candidate;
                queue.emplace(candidate, arc.to);
            }
        }
        return std::nullopt;
    }

private:
    struct FlaggedArc{
        int to;
        Cost cost;
        std::vector<std::uint64_t> flags;
    };

    void preprocess_flags(const graph_type& graph){
        const int n = static_cast<int>(graph.size());
        if(n == 0) return;
        const graph_type reverse = road_routing_internal::reverse_graph(graph);
        for(int target = 0; target < n; ++target){
            const std::vector<Cost> distance =
                road_routing_internal::dijkstra(reverse, target);
            const int region =
                region_of_vertex_[static_cast<std::size_t>(target)];
            const std::size_t word = static_cast<std::size_t>(region) / 64U;
            const std::uint64_t bit =
                std::uint64_t{1} << (static_cast<unsigned>(region) & 63U);
            for(int from = 0; from < n; ++from){
                if(distance[static_cast<std::size_t>(from)]
                   == road_routing_internal::infinity<Cost>()){
                    continue;
                }
                for(std::size_t edge_index = 0;
                    edge_index < graph[static_cast<std::size_t>(from)].size();
                    ++edge_index){
                    const auto& [to, cost] =
                        graph[static_cast<std::size_t>(from)][edge_index];
                    if(distance[static_cast<std::size_t>(from)]
                       == road_routing_internal::add(
                           cost, distance[static_cast<std::size_t>(to)]
                       )){
                        graph_[static_cast<std::size_t>(from)][edge_index]
                            .flags[word] |= bit;
                    }
                }
            }
        }
    }

    std::vector<std::vector<FlaggedArc>> graph_;
    std::vector<int> region_of_vertex_;
    int region_count_ = 0;
};

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_ARC_FLAGS_HPP_INCLUDED
