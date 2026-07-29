#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_HUB_LABELING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_HUB_LABELING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "road_routing_common.hpp"

template<class Cost>
class HubLabeling{
public:
    using cost_type = Cost;
    using graph_type = road_routing_internal::Graph<Cost>;

    struct LabelEntry{
        int hub;
        Cost distance;
    };

    explicit HubLabeling(graph_type graph, std::vector<int> hub_order = {})
        : graph_(std::move(graph)){
        road_routing_internal::validate_graph(graph_);
        const int n = static_cast<int>(graph_.size());
        if(hub_order.empty()){
            hub_order.resize(static_cast<std::size_t>(n));
            std::iota(hub_order.begin(), hub_order.end(), 0);
            std::vector<std::size_t> degree(static_cast<std::size_t>(n), 0);
            for(int from = 0; from < n; ++from){
                degree[static_cast<std::size_t>(from)] +=
                    graph_[static_cast<std::size_t>(from)].size();
                for(const auto& edge : graph_[static_cast<std::size_t>(from)]){
                    ++degree[static_cast<std::size_t>(edge.first)];
                }
            }
            std::stable_sort(
                hub_order.begin(),
                hub_order.end(),
                [&degree](int lhs, int rhs){
                    return degree[static_cast<std::size_t>(rhs)]
                         < degree[static_cast<std::size_t>(lhs)];
                }
            );
        }
        validate_order(hub_order, n);
        order_position_.assign(static_cast<std::size_t>(n), -1);
        for(int position = 0; position < n; ++position){
            order_position_[static_cast<std::size_t>(
                hub_order[static_cast<std::size_t>(position)]
            )] = position;
        }
        forward_labels_.assign(static_cast<std::size_t>(n), {});
        backward_labels_.assign(static_cast<std::size_t>(n), {});
        build(hub_order);
    }

    [[nodiscard]] int vertex_count() const noexcept{
        return static_cast<int>(graph_.size());
    }

    [[nodiscard]] const std::vector<LabelEntry>& forward_labels(
        int vertex
    ) const{
        road_routing_internal::validate_vertex(vertex, vertex_count());
        return forward_labels_[static_cast<std::size_t>(vertex)];
    }

    [[nodiscard]] const std::vector<LabelEntry>& backward_labels(
        int vertex
    ) const{
        road_routing_internal::validate_vertex(vertex, vertex_count());
        return backward_labels_[static_cast<std::size_t>(vertex)];
    }

    [[nodiscard]] std::optional<Cost> query_distance(
        int source,
        int target
    ) const{
        const int n = vertex_count();
        road_routing_internal::validate_vertex(source, n);
        road_routing_internal::validate_vertex(target, n);
        if(source == target) return Cost{};
        return road_routing_internal::optional_distance(
            label_distance(source, target)
        );
    }

private:
    static void validate_order(const std::vector<int>& order, int n){
        if(static_cast<int>(order.size()) != n){
            throw std::invalid_argument(
                "HubLabeling order must contain every vertex"
            );
        }
        std::vector<char> seen(static_cast<std::size_t>(n), 0);
        for(const int vertex : order){
            if(vertex < 0 || n <= vertex
               || seen[static_cast<std::size_t>(vertex)]){
                throw std::invalid_argument(
                    "HubLabeling order must be a permutation"
                );
            }
            seen[static_cast<std::size_t>(vertex)] = 1;
        }
    }

    Cost label_distance(int source, int target) const noexcept{
        const auto& forward =
            forward_labels_[static_cast<std::size_t>(source)];
        const auto& backward =
            backward_labels_[static_cast<std::size_t>(target)];
        Cost best = road_routing_internal::infinity<Cost>();
        std::size_t first = 0;
        std::size_t second = 0;
        while(first < forward.size() && second < backward.size()){
            const int first_position = order_position_[static_cast<std::size_t>(
                forward[first].hub
            )];
            const int second_position = order_position_[static_cast<std::size_t>(
                backward[second].hub
            )];
            if(first_position < second_position){
                ++first;
            }else if(second_position < first_position){
                ++second;
            }else{
                const Cost candidate = road_routing_internal::add(
                    forward[first].distance, backward[second].distance
                );
                if(candidate < best) best = candidate;
                ++first;
                ++second;
            }
        }
        return best;
    }

    void build(const std::vector<int>& order){
        const graph_type reverse =
            road_routing_internal::reverse_graph(graph_);
        for(const int hub : order){
            pruned_dijkstra(hub, graph_, false);
            pruned_dijkstra(hub, reverse, true);
        }
    }

    void pruned_dijkstra(
        int hub,
        const graph_type& search_graph,
        bool reverse_search
    ){
        const int n = vertex_count();
        const Cost inf = road_routing_internal::infinity<Cost>();
        std::vector<Cost> distance(static_cast<std::size_t>(n), inf);
        using Entry = std::pair<Cost, int>;
        std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
        distance[static_cast<std::size_t>(hub)] = Cost{};
        queue.emplace(Cost{}, hub);
        while(!queue.empty()){
            const auto [current_distance, vertex] = queue.top();
            queue.pop();
            if(distance[static_cast<std::size_t>(vertex)] != current_distance){
                continue;
            }
            const Cost covered = reverse_search
                ? label_distance(vertex, hub)
                : label_distance(hub, vertex);
            if(covered <= current_distance) continue;
            if(reverse_search){
                forward_labels_[static_cast<std::size_t>(vertex)].push_back(
                    LabelEntry{hub, current_distance}
                );
            }else{
                backward_labels_[static_cast<std::size_t>(vertex)].push_back(
                    LabelEntry{hub, current_distance}
                );
            }
            for(const auto& [to, cost] :
                search_graph[static_cast<std::size_t>(vertex)]){
                const Cost candidate =
                    road_routing_internal::add(current_distance, cost);
                if(!(candidate < distance[static_cast<std::size_t>(to)])){
                    continue;
                }
                distance[static_cast<std::size_t>(to)] = candidate;
                queue.emplace(candidate, to);
            }
        }
    }

    graph_type graph_;
    std::vector<int> order_position_;
    std::vector<std::vector<LabelEntry>> forward_labels_;
    std::vector<std::vector<LabelEntry>> backward_labels_;
};

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_HUB_LABELING_HPP_INCLUDED
