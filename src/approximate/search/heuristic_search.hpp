#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <optional>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

template<class State, class Cost>
struct HeuristicSearchResult{
    std::optional<Cost> cost;
    std::vector<State> path;
    std::size_t expanded = 0;

    explicit operator bool() const noexcept{
        return cost.has_value();
    }
};

namespace heuristic_search_internal{

template<class Range>
using range_value_t = std::decay_t<decltype(
    *std::begin(std::declval<std::remove_reference_t<Range>&>())
)>;

template<class State, class Cost>
struct Node{
    State state;
    Cost distance;
    std::size_t parent = 0;
    bool has_parent = false;
};

template<class State, class Cost>
std::vector<State> restore_path(
    const std::vector<Node<State, Cost>>& nodes,
    std::size_t index
){
    std::vector<State> path;
    while(true){
        path.push_back(nodes[index].state);
        if(!nodes[index].has_parent) break;
        index = nodes[index].parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Weight, class Hash, class Equal>
auto weighted_a_star_impl(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Weight weight,
    Hash hash,
    Equal equal
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using HeuristicValue = std::decay_t<std::invoke_result_t<Heuristic&, const State&>>;
    using Priority = std::decay_t<decltype(
        std::declval<Cost>() + std::declval<Weight>() * std::declval<HeuristicValue>()
    )>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct QueueEntry{
        Priority priority;
        Cost distance;
        std::size_t order;
        std::size_t node;
    };
    struct Compare{
        bool operator()(const QueueEntry& lhs, const QueueEntry& rhs) const{
            if(rhs.priority < lhs.priority) return true;
            if(lhs.priority < rhs.priority) return false;
            return rhs.order < lhs.order;
        }
    };

    if(weight < Weight{})[[unlikely]]{
        throw std::invalid_argument("weighted_a_star_search requires a non-negative weight");
    }

    std::vector<Node<State, Cost>> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(0, hash, equal);
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, Compare> queue;
    nodes.push_back(Node<State, Cost>{start, Cost{}, 0, false});
    index.emplace(start, 0);
    std::size_t order = 0;
    queue.push(QueueEntry{
        static_cast<Priority>(weight)
            * static_cast<Priority>(std::invoke(heuristic, start)),
        Cost{}, order++, 0
    });

    Result result;
    while(!queue.empty()){
        const QueueEntry entry = queue.top();
        queue.pop();
        if(entry.distance != nodes[entry.node].distance) continue;

        ++result.expanded;
        const State current_state = nodes[entry.node].state;
        const Cost current_distance = nodes[entry.node].distance;
        if(static_cast<bool>(std::invoke(goal, current_state))){
            result.cost = current_distance;
            result.path = restore_path(nodes, entry.node);
            return result;
        }

        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument("heuristic search does not accept negative edges");
            }
            const Cost candidate = current_distance + edge_cost;
            const State& next_state = edge.first;
            auto iterator = index.find(next_state);
            std::size_t next_index;
            if(iterator == index.end()){
                next_index = nodes.size();
                nodes.push_back(Node<State, Cost>{next_state, candidate, entry.node, true});
                index.emplace(nodes.back().state, next_index);
            }else{
                next_index = iterator->second;
                if(!(candidate < nodes[next_index].distance)) continue;
                nodes[next_index].distance = candidate;
                nodes[next_index].parent = entry.node;
                nodes[next_index].has_parent = true;
            }
            const auto estimate = std::invoke(heuristic, nodes[next_index].state);
            queue.push(QueueEntry{
                static_cast<Priority>(candidate)
                    + static_cast<Priority>(weight)
                        * static_cast<Priority>(estimate),
                candidate, order++, next_index
            });
        }
    }
    return result;
}

} // namespace heuristic_search_internal

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Hash hash = {},
    Equal equal = {}
){
    return heuristic_search_internal::weighted_a_star_impl(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        1, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic, class Weight,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto weighted_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Weight weight,
    Hash hash = {},
    Equal equal = {}
){
    return heuristic_search_internal::weighted_a_star_impl(
        start, std::move(goal), std::move(neighbors), std::move(heuristic),
        weight, std::move(hash), std::move(equal)
    );
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto greedy_best_first_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Priority = std::decay_t<std::invoke_result_t<Heuristic&, const State&>>;
    using Node = heuristic_search_internal::Node<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct QueueEntry{
        Priority priority;
        std::size_t order;
        std::size_t node;
    };
    struct Compare{
        bool operator()(const QueueEntry& lhs, const QueueEntry& rhs) const{
            if(rhs.priority < lhs.priority) return true;
            if(lhs.priority < rhs.priority) return false;
            return rhs.order < lhs.order;
        }
    };

    std::vector<Node> nodes;
    std::unordered_map<State, std::size_t, Hash, Equal> index(0, hash, equal);
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, Compare> queue;
    nodes.push_back(Node{start, Cost{}, 0, false});
    index.emplace(start, 0);
    std::size_t order = 0;
    queue.push(QueueEntry{std::invoke(heuristic, start), order++, 0});

    Result result;
    while(!queue.empty()){
        const QueueEntry entry = queue.top();
        queue.pop();
        ++result.expanded;
        const State current_state = nodes[entry.node].state;
        if(static_cast<bool>(std::invoke(goal, current_state))){
            result.cost = nodes[entry.node].distance;
            result.path = heuristic_search_internal::restore_path(nodes, entry.node);
            return result;
        }

        auto adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument("greedy_best_first_search does not accept negative edges");
            }
            const State& next_state = edge.first;
            if(index.find(next_state) != index.end()) continue;
            const std::size_t next_index = nodes.size();
            nodes.push_back(Node{
                next_state, nodes[entry.node].distance + edge_cost, entry.node, true
            });
            index.emplace(nodes.back().state, next_index);
            queue.push(QueueEntry{
                std::invoke(heuristic, nodes.back().state), order++, next_index
            });
        }
    }
    return result;
}

template<class State, class ForwardNeighbors, class BackwardNeighbors,
         class ForwardHeuristic, class BackwardHeuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto bidirectional_a_star_search(
    const State& start,
    const State& goal,
    ForwardNeighbors forward_neighbors,
    BackwardNeighbors backward_neighbors,
    ForwardHeuristic forward_heuristic,
    BackwardHeuristic backward_heuristic,
    Hash hash = {},
    Equal equal = {}
){
    using ForwardRange = std::invoke_result_t<ForwardNeighbors&, const State&>;
    using ForwardEdge = heuristic_search_internal::range_value_t<ForwardRange>;
    using Cost = std::decay_t<decltype(std::declval<ForwardEdge>().second)>;
    using ForwardH = std::decay_t<std::invoke_result_t<ForwardHeuristic&, const State&>>;
    using BackwardH = std::decay_t<std::invoke_result_t<BackwardHeuristic&, const State&>>;
    using Priority = std::common_type_t<Cost, ForwardH, BackwardH>;
    using Node = heuristic_search_internal::Node<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct QueueEntry{
        Priority priority;
        Cost distance;
        std::size_t order;
        std::size_t node;
    };
    struct Compare{
        bool operator()(const QueueEntry& lhs, const QueueEntry& rhs) const{
            if(rhs.priority < lhs.priority) return true;
            if(lhs.priority < rhs.priority) return false;
            return rhs.order < lhs.order;
        }
    };
    struct Side{
        std::vector<Node> nodes;
        std::unordered_map<State, std::size_t, Hash, Equal> index;
        std::priority_queue<QueueEntry, std::vector<QueueEntry>, Compare> queue;

        Side(Hash side_hash, Equal side_equal)
            : index(0, std::move(side_hash), std::move(side_equal)){}
    };

    Result result;
    if(equal(start, goal)){
        result.cost = Cost{};
        result.path.push_back(start);
        return result;
    }

    Side forward(hash, equal);
    Side backward(hash, equal);
    forward.nodes.push_back(Node{start, Cost{}, 0, false});
    backward.nodes.push_back(Node{goal, Cost{}, 0, false});
    forward.index.emplace(start, 0);
    backward.index.emplace(goal, 0);
    std::size_t order = 0;
    forward.queue.push(QueueEntry{
        static_cast<Priority>(std::invoke(forward_heuristic, start)), Cost{}, order++, 0
    });
    backward.queue.push(QueueEntry{
        static_cast<Priority>(std::invoke(backward_heuristic, goal)), Cost{}, order++, 0
    });

    std::optional<Cost> best;
    std::size_t meeting_forward = 0;
    std::size_t meeting_backward = 0;

    const auto discard_stale = [](Side& side){
        while(!side.queue.empty()
              && side.queue.top().distance != side.nodes[side.queue.top().node].distance){
            side.queue.pop();
        }
    };
    const auto update_meeting = [&](std::size_t own_index, bool own_is_forward){
        Side& own = own_is_forward ? forward : backward;
        Side& other = own_is_forward ? backward : forward;
        const auto iterator = other.index.find(own.nodes[own_index].state);
        if(iterator == other.index.end()) return;
        const Cost candidate = own.nodes[own_index].distance
                             + other.nodes[iterator->second].distance;
        if(best && !(candidate < *best)) return;
        best = candidate;
        if(own_is_forward){
            meeting_forward = own_index;
            meeting_backward = iterator->second;
        }else{
            meeting_forward = iterator->second;
            meeting_backward = own_index;
        }
    };

    while(true){
        discard_stale(forward);
        discard_stale(backward);
        if(forward.queue.empty() && backward.queue.empty()) break;
        if(best){
            const bool forward_done = forward.queue.empty()
                || !((forward.queue.top().priority) < static_cast<Priority>(*best));
            const bool backward_done = backward.queue.empty()
                || !((backward.queue.top().priority) < static_cast<Priority>(*best));
            if(forward_done && backward_done) break;
        }

        const bool expand_forward = backward.queue.empty()
            || (!forward.queue.empty()
                && !(backward.queue.top().priority < forward.queue.top().priority));
        const auto expand_side = [&](Side& own, auto& neighbor_function,
                                     auto& heuristic_function, bool own_is_forward){
            const QueueEntry entry = own.queue.top();
            own.queue.pop();
            ++result.expanded;
            update_meeting(entry.node, own_is_forward);

            const State current_state = own.nodes[entry.node].state;
            const Cost current_distance = own.nodes[entry.node].distance;
            auto adjacent = std::invoke(neighbor_function, current_state);
            for(const auto& edge : adjacent){
                const Cost edge_cost = static_cast<Cost>(edge.second);
                if(edge_cost < Cost{})[[unlikely]]{
                    throw std::invalid_argument(
                        "bidirectional_a_star_search does not accept negative edges"
                    );
                }
                const Cost candidate = current_distance + edge_cost;
                const State& next_state = edge.first;
                auto iterator = own.index.find(next_state);
                std::size_t next_index;
                if(iterator == own.index.end()){
                    next_index = own.nodes.size();
                    own.nodes.push_back(Node{next_state, candidate, entry.node, true});
                    own.index.emplace(own.nodes.back().state, next_index);
                }else{
                    next_index = iterator->second;
                    if(!(candidate < own.nodes[next_index].distance)) continue;
                    own.nodes[next_index].distance = candidate;
                    own.nodes[next_index].parent = entry.node;
                    own.nodes[next_index].has_parent = true;
                }
                const Priority estimate = static_cast<Priority>(
                    std::invoke(heuristic_function, own.nodes[next_index].state)
                );
                own.queue.push(QueueEntry{
                    static_cast<Priority>(candidate) + estimate,
                    candidate, order++, next_index
                });
                update_meeting(next_index, own_is_forward);
            }
        };
        if(expand_forward){
            expand_side(forward, forward_neighbors, forward_heuristic, true);
        }else{
            expand_side(backward, backward_neighbors, backward_heuristic, false);
        }
    }

    if(!best) return result;
    result.cost = best;
    result.path = heuristic_search_internal::restore_path(forward.nodes, meeting_forward);
    std::size_t index = meeting_backward;
    while(backward.nodes[index].has_parent){
        index = backward.nodes[index].parent;
        result.path.push_back(backward.nodes[index].state);
    }
    return result;
}
