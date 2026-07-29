#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_NEAR_OPTIMAL_BIDIRECTIONAL_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_NEAR_OPTIMAL_BIDIRECTIONAL_SEARCH_HPP_INCLUDED

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

#include "heuristic_search.hpp"

namespace near_optimal_bidirectional_search_internal{

enum class Location : unsigned char{
    waiting,
    ready,
    closed
};

template<class State, class Cost, class Priority>
struct Node{
    State state;
    Cost distance;
    Priority heuristic;
    std::size_t parent = 0;
    std::size_t version = 0;
    bool has_parent = false;
    Location location = Location::waiting;
};

template<class State, class Cost, class Priority>
std::vector<State> restore_path(
    const std::vector<Node<State, Cost, Priority>>& nodes,
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

} // namespace near_optimal_bidirectional_search_internal

// Near-optimal Bidirectional Search (NBS).  Waiting queues are ordered by f;
// nodes admitted by the current lower bound are paired through ready g-queues.
template<class State, class ForwardNeighbors, class BackwardNeighbors,
         class ForwardHeuristic, class BackwardHeuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto nbs_search(
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
    using ForwardEstimate = std::decay_t<std::invoke_result_t<
        ForwardHeuristic&, const State&
    >>;
    using BackwardEstimate = std::decay_t<std::invoke_result_t<
        BackwardHeuristic&, const State&
    >>;
    using Priority = std::common_type_t<Cost, ForwardEstimate, BackwardEstimate>;
    using Location = near_optimal_bidirectional_search_internal::Location;
    using Node = near_optimal_bidirectional_search_internal::Node<
        State, Cost, Priority
    >;
    using Result = HeuristicSearchResult<State, Cost>;

    struct Entry{
        Priority key;
        Cost distance;
        std::size_t order;
        std::size_t node;
        std::size_t version;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            if(rhs.key < lhs.key) return true;
            if(lhs.key < rhs.key) return false;
            return rhs.order < lhs.order;
        }
    };
    struct Side{
        std::vector<Node> nodes;
        std::unordered_map<State, std::size_t, Hash, Equal> index;
        std::priority_queue<Entry, std::vector<Entry>, Compare> waiting;
        std::priority_queue<Entry, std::vector<Entry>, Compare> ready;

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
    std::size_t order = 0;
    const auto checked_estimate = [](const auto& value){
        const Priority estimate = static_cast<Priority>(value);
        if(estimate < Priority{})[[unlikely]]{
            throw std::invalid_argument(
                "nbs_search requires non-negative heuristic estimates"
            );
        }
        return estimate;
    };
    const Priority start_h = checked_estimate(
        std::invoke(forward_heuristic, start)
    );
    const Priority goal_h = checked_estimate(
        std::invoke(backward_heuristic, goal)
    );
    forward.nodes.push_back(Node{start, Cost{}, start_h});
    backward.nodes.push_back(Node{goal, Cost{}, goal_h});
    forward.index.emplace(start, 0);
    backward.index.emplace(goal, 0);

    const auto push_waiting = [&](Side& side, std::size_t node_index){
        Node& node = side.nodes[node_index];
        node.location = Location::waiting;
        side.waiting.push(Entry{
            static_cast<Priority>(node.distance) + node.heuristic,
            node.distance, order++, node_index, node.version
        });
    };
    const auto push_ready = [&](Side& side, std::size_t node_index){
        Node& node = side.nodes[node_index];
        node.location = Location::ready;
        side.ready.push(Entry{
            static_cast<Priority>(node.distance),
            node.distance, order++, node_index, node.version
        });
    };
    push_waiting(forward, 0);
    push_waiting(backward, 0);

    const auto clean_waiting = [](Side& side){
        while(!side.waiting.empty()){
            const Entry& entry = side.waiting.top();
            const Node& node = side.nodes[entry.node];
            if(node.location == Location::waiting
               && node.version == entry.version
               && node.distance == entry.distance){
                break;
            }
            side.waiting.pop();
        }
    };
    const auto clean_ready = [](Side& side){
        while(!side.ready.empty()){
            const Entry& entry = side.ready.top();
            const Node& node = side.nodes[entry.node];
            if(node.location == Location::ready
               && node.version == entry.version
               && node.distance == entry.distance){
                break;
            }
            side.ready.pop();
        }
    };

    std::optional<Cost> incumbent;
    std::size_t meeting_forward = 0;
    std::size_t meeting_backward = 0;
    const auto update_meeting = [&](std::size_t own_index, bool is_forward){
        Side& own = is_forward ? forward : backward;
        Side& other = is_forward ? backward : forward;
        const auto iterator = other.index.find(own.nodes[own_index].state);
        if(iterator == other.index.end()) return;
        const Cost candidate = own.nodes[own_index].distance
                             + other.nodes[iterator->second].distance;
        if(incumbent && !(candidate < *incumbent)) return;
        incumbent = candidate;
        if(is_forward){
            meeting_forward = own_index;
            meeting_backward = iterator->second;
        }else{
            meeting_forward = iterator->second;
            meeting_backward = own_index;
        }
    };

    Priority lower_bound = std::max(start_h, goal_h);
    while(true){
        if(incumbent
           && !(lower_bound < static_cast<Priority>(*incumbent))){
            break;
        }

        const auto admit = [&](Side& side){
            clean_waiting(side);
            while(!side.waiting.empty()
                  && !(lower_bound < side.waiting.top().key)){
                const Entry entry = side.waiting.top();
                side.waiting.pop();
                push_ready(side, entry.node);
                clean_waiting(side);
            }
            clean_ready(side);
        };
        admit(forward);
        admit(backward);

        if(!forward.ready.empty() && !backward.ready.empty()
           && !(lower_bound
                < forward.ready.top().key + backward.ready.top().key)){
            const std::size_t forward_index = forward.ready.top().node;
            const std::size_t backward_index = backward.ready.top().node;
            forward.ready.pop();
            backward.ready.pop();
            forward.nodes[forward_index].location = Location::closed;
            backward.nodes[backward_index].location = Location::closed;
            ++forward.nodes[forward_index].version;
            ++backward.nodes[backward_index].version;

            const auto expand = [&](Side& own, auto& neighbors, auto& heuristic,
                                    bool is_forward, std::size_t current_index){
                const State current_state = own.nodes[current_index].state;
                const Cost current_distance = own.nodes[current_index].distance;
                ++result.expanded;
                update_meeting(current_index, is_forward);

                auto adjacent = std::invoke(neighbors, current_state);
                for(const auto& edge : adjacent){
                    const Cost edge_cost = static_cast<Cost>(edge.second);
                    if(edge_cost < Cost{})[[unlikely]]{
                        throw std::invalid_argument(
                            "nbs_search does not accept negative edges"
                        );
                    }
                    const Cost candidate = current_distance + edge_cost;
                    const State& next_state = edge.first;
                    auto iterator = own.index.find(next_state);
                    std::size_t next_index;
                    if(iterator == own.index.end()){
                        const Priority estimate = checked_estimate(
                            std::invoke(heuristic, next_state)
                        );
                        next_index = own.nodes.size();
                        own.nodes.push_back(Node{
                            next_state, candidate, estimate,
                            current_index, 0, true, Location::waiting
                        });
                        own.index.emplace(own.nodes.back().state, next_index);
                    }else{
                        next_index = iterator->second;
                        Node& next = own.nodes[next_index];
                        if(!(candidate < next.distance)) continue;
                        next.distance = candidate;
                        next.parent = current_index;
                        next.has_parent = true;
                        ++next.version;
                    }
                    update_meeting(next_index, is_forward);
                    Node& next = own.nodes[next_index];
                    const Priority f = static_cast<Priority>(next.distance)
                                     + next.heuristic;
                    if(!(lower_bound < f)){
                        push_ready(own, next_index);
                    }else{
                        push_waiting(own, next_index);
                    }
                }
            };
            expand(
                forward, forward_neighbors, forward_heuristic,
                true, forward_index
            );
            expand(
                backward, backward_neighbors, backward_heuristic,
                false, backward_index
            );
            continue;
        }

        clean_waiting(forward);
        clean_waiting(backward);
        clean_ready(forward);
        clean_ready(backward);
        std::optional<Priority> next_bound;
        const auto consider = [&](Priority candidate){
            if(!(lower_bound < candidate)) return;
            if(!next_bound || candidate < *next_bound) next_bound = candidate;
        };
        if(!forward.waiting.empty()) consider(forward.waiting.top().key);
        if(!backward.waiting.empty()) consider(backward.waiting.top().key);
        if(!forward.ready.empty() && !backward.ready.empty()){
            consider(forward.ready.top().key + backward.ready.top().key);
        }
        if(!next_bound) break;
        lower_bound = *next_bound;
    }

    if(!incumbent) return result;
    result.cost = incumbent;
    result.path = near_optimal_bidirectional_search_internal::restore_path(
        forward.nodes, meeting_forward
    );
    std::size_t index = meeting_backward;
    while(backward.nodes[index].has_parent){
        index = backward.nodes[index].parent;
        result.path.push_back(backward.nodes[index].state);
    }
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_NEAR_OPTIMAL_BIDIRECTIONAL_SEARCH_HPP_INCLUDED
