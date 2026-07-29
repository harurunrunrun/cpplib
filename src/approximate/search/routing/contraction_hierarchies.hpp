#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_CONTRACTION_HIERARCHIES_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_CONTRACTION_HIERARCHIES_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "road_routing_common.hpp"

template<class Cost>
class ContractionHierarchy{
public:
    using cost_type = Cost;
    using graph_type = road_routing_internal::Graph<Cost>;

    explicit ContractionHierarchy(
        graph_type graph,
        std::vector<int> contraction_order = {}
    ){
        road_routing_internal::validate_graph(graph);
        const int n = static_cast<int>(graph.size());
        if(contraction_order.empty()){
            contraction_order.resize(static_cast<std::size_t>(n));
            std::iota(contraction_order.begin(), contraction_order.end(), 0);
            std::stable_sort(
                contraction_order.begin(),
                contraction_order.end(),
                [&graph](int lhs, int rhs){
                    return graph[static_cast<std::size_t>(lhs)].size()
                         < graph[static_cast<std::size_t>(rhs)].size();
                }
            );
        }
        validate_order(contraction_order, n);
        rank_.assign(static_cast<std::size_t>(n), -1);
        for(int rank = 0; rank < n; ++rank){
            rank_[static_cast<std::size_t>(
                contraction_order[static_cast<std::size_t>(rank)]
            )] = rank;
        }
        build(std::move(graph), contraction_order);
    }

    [[nodiscard]] int vertex_count() const noexcept{
        return static_cast<int>(rank_.size());
    }

    [[nodiscard]] int rank(int vertex) const{
        road_routing_internal::validate_vertex(vertex, vertex_count());
        return rank_[static_cast<std::size_t>(vertex)];
    }

    [[nodiscard]] std::size_t shortcut_count() const noexcept{
        return shortcut_count_;
    }

    [[nodiscard]] std::optional<Cost> query_distance(
        int source,
        int target
    ) const{
        const int n = vertex_count();
        road_routing_internal::validate_vertex(source, n);
        road_routing_internal::validate_vertex(target, n);
        if(source == target) return Cost{};
        const Cost inf = road_routing_internal::infinity<Cost>();
        std::vector<Cost> forward_distance(static_cast<std::size_t>(n), inf);
        std::vector<Cost> backward_distance(static_cast<std::size_t>(n), inf);
        using Entry = std::pair<Cost, int>;
        std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>>
            forward_queue;
        std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>>
            backward_queue;
        forward_distance[static_cast<std::size_t>(source)] = Cost{};
        backward_distance[static_cast<std::size_t>(target)] = Cost{};
        forward_queue.emplace(Cost{}, source);
        backward_queue.emplace(Cost{}, target);
        Cost best = inf;

        const auto remove_stale = [](auto& queue, const auto& distance){
            while(!queue.empty()
                  && distance[static_cast<std::size_t>(queue.top().second)]
                     != queue.top().first){
                queue.pop();
            }
        };
        while(!forward_queue.empty() || !backward_queue.empty()){
            remove_stale(forward_queue, forward_distance);
            remove_stale(backward_queue, backward_distance);
            const Cost forward_min =
                forward_queue.empty() ? inf : forward_queue.top().first;
            const Cost backward_min =
                backward_queue.empty() ? inf : backward_queue.top().first;
            if(best != inf && !(forward_min < best) && !(backward_min < best)){
                break;
            }
            if(backward_min < forward_min){
                settle_one(
                    backward_queue,
                    backward_distance,
                    forward_distance,
                    backward_upward_,
                    best
                );
            }else{
                settle_one(
                    forward_queue,
                    forward_distance,
                    backward_distance,
                    forward_upward_,
                    best
                );
            }
        }
        return road_routing_internal::optional_distance(best);
    }

private:
    struct Arc{
        int from;
        int to;
        Cost cost;
        bool shortcut;
    };

    static void validate_order(const std::vector<int>& order, int n){
        if(static_cast<int>(order.size()) != n){
            throw std::invalid_argument(
                "ContractionHierarchy order must contain every vertex"
            );
        }
        std::vector<char> seen(static_cast<std::size_t>(n), 0);
        for(const int vertex : order){
            if(vertex < 0 || n <= vertex
               || seen[static_cast<std::size_t>(vertex)]){
                throw std::invalid_argument(
                    "ContractionHierarchy order must be a permutation"
                );
            }
            seen[static_cast<std::size_t>(vertex)] = 1;
        }
    }

    static void settle_one(
        std::priority_queue<
            std::pair<Cost, int>,
            std::vector<std::pair<Cost, int>>,
            std::greater<std::pair<Cost, int>>
        >& queue,
        std::vector<Cost>& own_distance,
        const std::vector<Cost>& opposite_distance,
        const graph_type& upward,
        Cost& best
    ){
        if(queue.empty()) return;
        const auto [current_distance, vertex] = queue.top();
        queue.pop();
        if(own_distance[static_cast<std::size_t>(vertex)] != current_distance){
            return;
        }
        const Cost meeting = road_routing_internal::add(
            current_distance,
            opposite_distance[static_cast<std::size_t>(vertex)]
        );
        if(meeting < best) best = meeting;
        for(const auto& [to, cost] : upward[static_cast<std::size_t>(vertex)]){
            const Cost candidate =
                road_routing_internal::add(current_distance, cost);
            if(candidate < own_distance[static_cast<std::size_t>(to)]){
                own_distance[static_cast<std::size_t>(to)] = candidate;
                queue.emplace(candidate, to);
            }
            const Cost joined = road_routing_internal::add(
                candidate, opposite_distance[static_cast<std::size_t>(to)]
            );
            if(joined < best) best = joined;
        }
    }

    void build(graph_type graph, const std::vector<int>& order){
        const int n = static_cast<int>(graph.size());
        std::vector<Arc> arcs;
        std::vector<std::vector<int>> outgoing(static_cast<std::size_t>(n));
        std::vector<std::vector<int>> incoming(static_cast<std::size_t>(n));
        const auto add_arc = [&](int from, int to, Cost cost, bool shortcut){
            const int index = static_cast<int>(arcs.size());
            arcs.push_back(Arc{from, to, cost, shortcut});
            outgoing[static_cast<std::size_t>(from)].push_back(index);
            incoming[static_cast<std::size_t>(to)].push_back(index);
        };
        for(int from = 0; from < n; ++from){
            for(const auto& [to, cost] : graph[static_cast<std::size_t>(from)]){
                add_arc(from, to, cost, false);
            }
        }

        std::vector<char> contracted(static_cast<std::size_t>(n), 0);
        for(const int vertex : order){
            std::vector<std::tuple<int, int, Cost>> candidates;
            for(const int incoming_index :
                incoming[static_cast<std::size_t>(vertex)]){
                const Arc& first = arcs[static_cast<std::size_t>(incoming_index)];
                if(contracted[static_cast<std::size_t>(first.from)]) continue;
                for(const int outgoing_index :
                    outgoing[static_cast<std::size_t>(vertex)]){
                    const Arc& second =
                        arcs[static_cast<std::size_t>(outgoing_index)];
                    if(contracted[static_cast<std::size_t>(second.to)]
                       || first.from == second.to){
                        continue;
                    }
                    candidates.emplace_back(
                        first.from,
                        second.to,
                        road_routing_internal::add(first.cost, second.cost)
                    );
                }
            }
            std::sort(candidates.begin(), candidates.end());
            std::vector<std::tuple<int, int, Cost>> unique_candidates;
            for(const auto& candidate : candidates){
                if(std::get<2>(candidate)
                   == road_routing_internal::infinity<Cost>()){
                    continue;
                }
                if(!unique_candidates.empty()
                   && std::get<0>(unique_candidates.back())
                          == std::get<0>(candidate)
                   && std::get<1>(unique_candidates.back())
                          == std::get<1>(candidate)){
                    if(std::get<2>(candidate)
                       < std::get<2>(unique_candidates.back())){
                        std::get<2>(unique_candidates.back()) =
                            std::get<2>(candidate);
                    }
                }else{
                    unique_candidates.push_back(candidate);
                }
            }
            for(const auto& [from, to, cost] : unique_candidates){
                if(!has_witness(
                    from, to, vertex, cost, contracted, arcs, outgoing
                )){
                    add_arc(from, to, cost, true);
                    ++shortcut_count_;
                }
            }
            contracted[static_cast<std::size_t>(vertex)] = 1;
        }

        forward_upward_.assign(static_cast<std::size_t>(n), {});
        backward_upward_.assign(static_cast<std::size_t>(n), {});
        for(const Arc& arc : arcs){
            const int from_rank = rank_[static_cast<std::size_t>(arc.from)];
            const int to_rank = rank_[static_cast<std::size_t>(arc.to)];
            if(from_rank < to_rank){
                forward_upward_[static_cast<std::size_t>(arc.from)]
                    .emplace_back(arc.to, arc.cost);
            }else if(to_rank < from_rank){
                backward_upward_[static_cast<std::size_t>(arc.to)]
                    .emplace_back(arc.from, arc.cost);
            }
        }
    }

    static bool has_witness(
        int source,
        int target,
        int avoided,
        Cost limit,
        const std::vector<char>& contracted,
        const std::vector<Arc>& arcs,
        const std::vector<std::vector<int>>& outgoing
    ){
        const int n = static_cast<int>(contracted.size());
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
            if(limit < current_distance) break;
            if(vertex == target) return true;
            for(const int arc_index : outgoing[static_cast<std::size_t>(vertex)]){
                const Arc& arc = arcs[static_cast<std::size_t>(arc_index)];
                if(arc.to == avoided
                   || contracted[static_cast<std::size_t>(arc.to)]){
                    continue;
                }
                const Cost candidate =
                    road_routing_internal::add(current_distance, arc.cost);
                if(limit < candidate
                   || !(candidate < distance[static_cast<std::size_t>(arc.to)])){
                    continue;
                }
                distance[static_cast<std::size_t>(arc.to)] = candidate;
                queue.emplace(candidate, arc.to);
            }
        }
        return false;
    }

    std::vector<int> rank_;
    graph_type forward_upward_;
    graph_type backward_upward_;
    std::size_t shortcut_count_ = 0;
};

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ROUTING_CONTRACTION_HIERARCHIES_HPP_INCLUDED
