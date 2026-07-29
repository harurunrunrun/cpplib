#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_MM_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_MM_SEARCH_HPP_INCLUDED

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

namespace mm_search_internal{

template<class State, class Cost, class Priority>
struct Node{
    State state;
    Cost distance;
    Priority heuristic;
    std::size_t parent = 0;
    std::size_t version = 0;
    bool has_parent = false;
    bool open = true;
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

} // namespace mm_search_internal

// Meet-in-the-Middle (MM) search.  Its priority max(g+h, 2g) prevents either
// direction from expanding beyond the midpoint before it is necessary.
template<class State, class ForwardNeighbors, class BackwardNeighbors,
         class ForwardHeuristic, class BackwardHeuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto mm_search(
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
    using Node = mm_search_internal::Node<State, Cost, Priority>;
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
        std::priority_queue<Entry, std::vector<Entry>, Compare> by_priority;
        std::priority_queue<Entry, std::vector<Entry>, Compare> by_f;
        std::priority_queue<Entry, std::vector<Entry>, Compare> by_g;

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
                "mm_search requires non-negative heuristic estimates"
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

    const auto push = [&](Side& side, std::size_t node_index){
        const Node& node = side.nodes[node_index];
        const Priority g = static_cast<Priority>(node.distance);
        const Priority f = g + node.heuristic;
        const Priority priority = std::max(f, Priority{2} * g);
        side.by_priority.push(Entry{
            priority, node.distance, order++, node_index, node.version
        });
        side.by_f.push(Entry{
            f, node.distance, order++, node_index, node.version
        });
        side.by_g.push(Entry{
            g, node.distance, order++, node_index, node.version
        });
    };
    push(forward, 0);
    push(backward, 0);

    const auto entry_is_current = [](const Side& side, const Entry& entry){
        const Node& node = side.nodes[entry.node];
        return node.open
            && node.version == entry.version
            && node.distance == entry.distance;
    };
    const auto clean = [&](Side& side){
        const auto clean_queue = [&](auto& queue){
            while(!queue.empty() && !entry_is_current(side, queue.top())){
                queue.pop();
            }
        };
        clean_queue(side.by_priority);
        clean_queue(side.by_f);
        clean_queue(side.by_g);
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

    while(true){
        clean(forward);
        clean(backward);
        if(forward.by_priority.empty() || backward.by_priority.empty()) break;

        const Priority minimum_priority = std::min(
            forward.by_priority.top().key,
            backward.by_priority.top().key
        );
        const Priority lower_bound = std::max({
            minimum_priority,
            forward.by_f.top().key,
            backward.by_f.top().key,
            forward.by_g.top().key + backward.by_g.top().key
        });
        if(incumbent
           && !(lower_bound < static_cast<Priority>(*incumbent))){
            break;
        }

        const bool expand_forward = !(
            backward.by_priority.top().key
            < forward.by_priority.top().key
        );
        const auto expand = [&](Side& own, auto& neighbors, auto& heuristic,
                                bool is_forward){
            const Entry entry = own.by_priority.top();
            own.by_priority.pop();
            Node& selected = own.nodes[entry.node];
            selected.open = false;
            ++selected.version;
            const State current_state = selected.state;
            const Cost current_distance = selected.distance;
            ++result.expanded;
            update_meeting(entry.node, is_forward);

            auto adjacent = std::invoke(neighbors, current_state);
            for(const auto& edge : adjacent){
                const Cost edge_cost = static_cast<Cost>(edge.second);
                if(edge_cost < Cost{})[[unlikely]]{
                    throw std::invalid_argument(
                        "mm_search does not accept negative edges"
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
                        entry.node, 0, true, true
                    });
                    own.index.emplace(own.nodes.back().state, next_index);
                }else{
                    next_index = iterator->second;
                    Node& next = own.nodes[next_index];
                    if(!(candidate < next.distance)) continue;
                    next.distance = candidate;
                    next.parent = entry.node;
                    next.has_parent = true;
                    next.open = true;
                    ++next.version;
                }
                update_meeting(next_index, is_forward);
                push(own, next_index);
            }
        };

        if(expand_forward){
            expand(
                forward, forward_neighbors, forward_heuristic, true
            );
        }else{
            expand(
                backward, backward_neighbors, backward_heuristic, false
            );
        }
    }

    if(!incumbent) return result;
    result.cost = incumbent;
    result.path = mm_search_internal::restore_path(
        forward.nodes, meeting_forward
    );
    std::size_t index = meeting_backward;
    while(backward.nodes[index].has_parent){
        index = backward.nodes[index].parent;
        result.path.push_back(backward.nodes[index].state);
    }
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_MM_SEARCH_HPP_INCLUDED
