#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_TRANSIT_NODE_ROUTING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_TRANSIT_NODE_ROUTING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "road_routing_common.hpp"

template<class Cost>
class TransitNodeRouting{
public:
    using cost_type = Cost;
    using graph_type = road_routing_internal::Graph<Cost>;

    struct AccessNode{
        int transit_vertex;
        Cost distance;
    };

    TransitNodeRouting(graph_type graph, std::vector<int> transit_nodes)
        : graph_(std::move(graph)),
          transit_nodes_(std::move(transit_nodes)){
        road_routing_internal::validate_graph(graph_);
        validate_transit_nodes();
        preprocess();
    }

    [[nodiscard]] int vertex_count() const noexcept{
        return static_cast<int>(graph_.size());
    }

    [[nodiscard]] std::size_t transit_node_count() const noexcept{
        return transit_nodes_.size();
    }

    [[nodiscard]] const std::vector<int>& transit_nodes() const noexcept{
        return transit_nodes_;
    }

    [[nodiscard]] const std::vector<AccessNode>& forward_access_nodes(
        int vertex
    ) const{
        road_routing_internal::validate_vertex(vertex, vertex_count());
        return forward_access_[static_cast<std::size_t>(vertex)];
    }

    [[nodiscard]] const std::vector<AccessNode>& backward_access_nodes(
        int vertex
    ) const{
        road_routing_internal::validate_vertex(vertex, vertex_count());
        return backward_access_[static_cast<std::size_t>(vertex)];
    }

    [[nodiscard]] bool is_definitely_nonlocal(int source, int target) const{
        const int n = vertex_count();
        road_routing_internal::validate_vertex(source, n);
        road_routing_internal::validate_vertex(target, n);
        if(is_transit_[static_cast<std::size_t>(source)]
           || is_transit_[static_cast<std::size_t>(target)]){
            return true;
        }
        return component_[static_cast<std::size_t>(source)]
            != component_[static_cast<std::size_t>(target)];
    }

    [[nodiscard]] std::optional<Cost> query_distance(
        int source,
        int target
    ) const{
        const int n = vertex_count();
        road_routing_internal::validate_vertex(source, n);
        road_routing_internal::validate_vertex(target, n);
        if(source == target) return Cost{};
        Cost best = transit_distance(source, target);
        if(is_definitely_nonlocal(source, target)){
            return road_routing_internal::optional_distance(best);
        }
        best = local_distance(source, target, best);
        return road_routing_internal::optional_distance(best);
    }

private:
    void validate_transit_nodes(){
        const int n = vertex_count();
        is_transit_.assign(static_cast<std::size_t>(n), 0);
        transit_position_.assign(static_cast<std::size_t>(n), -1);
        for(const int vertex : transit_nodes_){
            if(vertex < 0 || n <= vertex
               || is_transit_[static_cast<std::size_t>(vertex)]){
                throw std::invalid_argument(
                    "TransitNodeRouting transit nodes must be distinct valid vertices"
                );
            }
            is_transit_[static_cast<std::size_t>(vertex)] = 1;
            transit_position_[static_cast<std::size_t>(vertex)] = static_cast<int>(std::find(transit_nodes_.begin(), transit_nodes_.end(), vertex) - transit_nodes_.begin());
        }
    }

    void preprocess(){
        const int n = vertex_count();
        const graph_type reverse = road_routing_internal::reverse_graph(graph_);
        from_transit_.reserve(transit_nodes_.size());
        to_transit_.reserve(transit_nodes_.size());
        for(const int transit : transit_nodes_){
            from_transit_.push_back(
                road_routing_internal::dijkstra(graph_, transit)
            );
            to_transit_.push_back(
                road_routing_internal::dijkstra(reverse, transit)
            );
        }
        forward_access_.assign(static_cast<std::size_t>(n), {});
        backward_access_.assign(static_cast<std::size_t>(n), {});
        const Cost inf = road_routing_internal::infinity<Cost>();
        for(int vertex = 0; vertex < n; ++vertex){
            for(std::size_t index = 0; index < transit_nodes_.size(); ++index){
                const Cost forward =
                    to_transit_[index][static_cast<std::size_t>(vertex)];
                if(forward != inf){
                    forward_access_[static_cast<std::size_t>(vertex)].push_back(
                        AccessNode{transit_nodes_[index], forward}
                    );
                }
                const Cost backward =
                    from_transit_[index][static_cast<std::size_t>(vertex)];
                if(backward != inf){
                    backward_access_[static_cast<std::size_t>(vertex)].push_back(
                        AccessNode{transit_nodes_[index], backward}
                    );
                }
            }
        }
        build_nontransit_components();
    }

    void build_nontransit_components(){
        const int n = vertex_count();
        std::vector<std::vector<int>> undirected(static_cast<std::size_t>(n));
        for(int from = 0; from < n; ++from){
            if(is_transit_[static_cast<std::size_t>(from)]) continue;
            for(const auto& edge : graph_[static_cast<std::size_t>(from)]){
                const int to = edge.first;
                if(is_transit_[static_cast<std::size_t>(to)]) continue;
                undirected[static_cast<std::size_t>(from)].push_back(to);
                undirected[static_cast<std::size_t>(to)].push_back(from);
            }
        }
        component_.assign(static_cast<std::size_t>(n), -1);
        int component_count = 0;
        std::vector<int> stack;
        for(int start = 0; start < n; ++start){
            if(is_transit_[static_cast<std::size_t>(start)]
               || component_[static_cast<std::size_t>(start)] != -1){
                continue;
            }
            component_[static_cast<std::size_t>(start)] = component_count;
            stack.push_back(start);
            while(!stack.empty()){
                const int vertex = stack.back();
                stack.pop_back();
                for(const int to : undirected[static_cast<std::size_t>(vertex)]){
                    if(component_[static_cast<std::size_t>(to)] != -1) continue;
                    component_[static_cast<std::size_t>(to)] = component_count;
                    stack.push_back(to);
                }
            }
            ++component_count;
        }
    }

    Cost transit_distance(int source, int target) const{
        Cost best = road_routing_internal::infinity<Cost>();
        const auto& first_access =
            forward_access_[static_cast<std::size_t>(source)];
        const auto& last_access =
            backward_access_[static_cast<std::size_t>(target)];
        for(const AccessNode& first : first_access){
            const std::size_t first_index = transit_index(first.transit_vertex);
            for(const AccessNode& last : last_access){
                const std::size_t last_index = transit_index(last.transit_vertex);
                const Cost via_table = road_routing_internal::add(
                    from_transit_[first_index][static_cast<std::size_t>(
                        transit_nodes_[last_index]
                    )],
                    last.distance
                );
                const Cost candidate =
                    road_routing_internal::add(first.distance, via_table);
                if(candidate < best) best = candidate;
            }
        }
        return best;
    }

    std::size_t transit_index(int vertex) const noexcept{
        return static_cast<std::size_t>(
            transit_position_[static_cast<std::size_t>(vertex)]
        );
    }

    Cost local_distance(int source, int target, Cost upper_bound) const{
        const int n = vertex_count();
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
            if(!(current_distance < upper_bound)) break;
            if(vertex == target) return current_distance;
            for(const auto& [to, cost] : graph_[static_cast<std::size_t>(vertex)]){
                if(is_transit_[static_cast<std::size_t>(to)]) continue;
                const Cost candidate =
                    road_routing_internal::add(current_distance, cost);
                if(!(candidate < upper_bound)
                   || !(candidate < distance[static_cast<std::size_t>(to)])){
                    continue;
                }
                distance[static_cast<std::size_t>(to)] = candidate;
                queue.emplace(candidate, to);
            }
        }
        return upper_bound;
    }

    graph_type graph_;
    std::vector<int> transit_nodes_;
    std::vector<char> is_transit_;
    std::vector<int> transit_position_;
    std::vector<std::vector<Cost>> from_transit_;
    std::vector<std::vector<Cost>> to_transit_;
    std::vector<std::vector<AccessNode>> forward_access_;
    std::vector<std::vector<AccessNode>> backward_access_;
    std::vector<int> component_;
};

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_TRANSIT_NODE_ROUTING_HPP_INCLUDED
