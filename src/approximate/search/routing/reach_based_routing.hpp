#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_REACH_BASED_ROUTING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_REACH_BASED_ROUTING_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <functional>
#include <optional>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "road_routing_common.hpp"

template<class Cost>
class ReachBasedRouting{
public:
    using cost_type = Cost;
    using graph_type = road_routing_internal::Graph<Cost>;

    // Builds safe reach upper bounds and exact target bounds.  This convenient
    // preprocessing is intended when an application has not built tighter
    // reach bounds itself.
    explicit ReachBasedRouting(graph_type graph)
        : graph_(std::move(graph)){
        road_routing_internal::validate_graph(graph_);
        target_lower_bounds_ =
            road_routing_internal::all_pairs_distances(graph_);
        preprocess_safe_reach_upper_bounds();
    }

    // Uses externally preprocessed safe upper bounds on vertex reach.  The
    // two-argument query remains correct (with a zero remaining-distance
    // bound); pass an admissible lower-bound callable to enable pruning.
    ReachBasedRouting(graph_type graph, std::vector<Cost> reach_upper_bounds)
        : graph_(std::move(graph)),
          reach_(std::move(reach_upper_bounds)){
        road_routing_internal::validate_graph(graph_);
        if(reach_.size() != graph_.size()){
            throw std::invalid_argument(
                "ReachBasedRouting needs one reach upper bound per vertex"
            );
        }
        for(const Cost value : reach_){
            bool invalid = false;
            if constexpr(std::is_signed_v<Cost>) invalid = value < Cost{};
            if constexpr(std::is_floating_point_v<Cost>){
                invalid = invalid || std::isnan(value);
            }
            if(invalid){
                throw std::invalid_argument(
                    "ReachBasedRouting reach upper bounds must be non-negative"
                );
            }
        }
    }

    [[nodiscard]] int vertex_count() const noexcept{
        return static_cast<int>(graph_.size());
    }

    [[nodiscard]] Cost reach_upper_bound(int vertex) const{
        road_routing_internal::validate_vertex(vertex, vertex_count());
        return reach_[static_cast<std::size_t>(vertex)];
    }

    [[nodiscard]] bool has_precomputed_target_bounds() const noexcept{
        return !target_lower_bounds_.empty();
    }

    [[nodiscard]] std::optional<Cost> query_distance(
        int source,
        int target
    ) const{
        if(has_precomputed_target_bounds()){
            return query_distance(
                source,
                target,
                [this](int vertex, int destination){
                    return target_lower_bounds_[static_cast<std::size_t>(vertex)]
                                               [static_cast<std::size_t>(
                                                   destination
                                               )];
                }
            );
        }
        return query_distance(
            source, target, [](int, int){ return Cost{}; }
        );
    }

    template<class RemainingDistanceLowerBound>
    [[nodiscard]] std::optional<Cost> query_distance(
        int source,
        int target,
        RemainingDistanceLowerBound remaining_distance_lower_bound
    ) const{
        const int n = vertex_count();
        road_routing_internal::validate_vertex(source, n);
        road_routing_internal::validate_vertex(target, n);
        if(source == target) return Cost{};
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
            const Cost remaining = static_cast<Cost>(std::invoke(
                remaining_distance_lower_bound, vertex, target
            ));
            bool invalid = false;
            if constexpr(std::is_signed_v<Cost>) invalid = remaining < Cost{};
            if constexpr(std::is_floating_point_v<Cost>){
                invalid = invalid || std::isnan(remaining);
            }
            if(invalid){
                throw std::invalid_argument(
                    "ReachBasedRouting lower bounds must be non-negative"
                );
            }
            const Cost vertex_reach =
                reach_[static_cast<std::size_t>(vertex)];
            if(vertex != source
               && vertex_reach < current_distance
               && vertex_reach < remaining){
                continue;
            }
            for(const auto& [to, cost] : graph_[static_cast<std::size_t>(vertex)]){
                const Cost candidate =
                    road_routing_internal::add(current_distance, cost);
                if(!(candidate < distance[static_cast<std::size_t>(to)])){
                    continue;
                }
                distance[static_cast<std::size_t>(to)] = candidate;
                queue.emplace(candidate, to);
            }
        }
        return std::nullopt;
    }

private:
    void preprocess_safe_reach_upper_bounds(){
        const int n = vertex_count();
        const Cost inf = road_routing_internal::infinity<Cost>();
        reach_.assign(static_cast<std::size_t>(n), Cost{});
        for(int vertex = 0; vertex < n; ++vertex){
            Cost maximum_prefix{};
            Cost maximum_suffix{};
            for(int endpoint = 0; endpoint < n; ++endpoint){
                const Cost prefix =
                    target_lower_bounds_[static_cast<std::size_t>(endpoint)]
                                        [static_cast<std::size_t>(vertex)];
                if(prefix != inf && maximum_prefix < prefix){
                    maximum_prefix = prefix;
                }
                const Cost suffix =
                    target_lower_bounds_[static_cast<std::size_t>(vertex)]
                                        [static_cast<std::size_t>(endpoint)];
                if(suffix != inf && maximum_suffix < suffix){
                    maximum_suffix = suffix;
                }
            }
            reach_[static_cast<std::size_t>(vertex)] =
                std::min(maximum_prefix, maximum_suffix);
        }
    }

    graph_type graph_;
    std::vector<Cost> reach_;
    std::vector<std::vector<Cost>> target_lower_bounds_;
};

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_REACH_BASED_ROUTING_HPP_INCLUDED
