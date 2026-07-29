#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_FRONT_TO_FRONT_BIDIRECTIONAL_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_FRONT_TO_FRONT_BIDIRECTIONAL_SEARCH_HPP_INCLUDED

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

namespace front_to_front_bidirectional_search_internal{

template<class State, class Cost>
struct Node{
    State state;
    Cost distance;
    std::size_t parent = 0;
    std::size_t version = 0;
    bool has_parent = false;
    bool open = true;
};

} // namespace front_to_front_bidirectional_search_internal

// The heuristic estimates the distance between two frontier states.  Unlike a
// conventional bidirectional A*, it is deliberately evaluated front-to-front.
template<class State, class ForwardNeighbors, class BackwardNeighbors,
         class FrontToFrontHeuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto front_to_front_bidirectional_search(
    const State& start,
    const State& goal,
    ForwardNeighbors forward_neighbors,
    BackwardNeighbors backward_neighbors,
    FrontToFrontHeuristic front_to_front_heuristic,
    Hash hash = {},
    Equal equal = {}
){
    using ForwardRange = std::invoke_result_t<ForwardNeighbors&, const State&>;
    using ForwardEdge = heuristic_search_internal::range_value_t<ForwardRange>;
    using Cost = std::decay_t<decltype(std::declval<ForwardEdge>().second)>;
    using Estimate = std::decay_t<std::invoke_result_t<
        FrontToFrontHeuristic&, const State&, const State&
    >>;
    using Priority = std::common_type_t<Cost, Estimate>;
    using Node = front_to_front_bidirectional_search_internal::Node<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct Side{
        std::vector<Node> nodes;
        std::unordered_map<State, std::size_t, Hash, Equal> index;

        Side(Hash side_hash, Equal side_equal)
            : index(0, std::move(side_hash), std::move(side_equal)){}
    };
    struct PairEntry{
        Priority lower_bound;
        std::size_t order;
        std::size_t forward_node;
        std::size_t backward_node;
        std::size_t forward_version;
        std::size_t backward_version;
    };
    struct PairCompare{
        bool operator()(const PairEntry& lhs, const PairEntry& rhs) const{
            if(rhs.lower_bound < lhs.lower_bound) return true;
            if(lhs.lower_bound < rhs.lower_bound) return false;
            return rhs.order < lhs.order;
        }
    };

    Result result;
    if(equal(start, goal)){
        result.cost = Cost{};
        result.path.push_back(start);
        return result;
    }

    Side forward(hash, equal);
    Side backward(hash, equal);
    forward.nodes.push_back(Node{start, Cost{}});
    backward.nodes.push_back(Node{goal, Cost{}});
    forward.index.emplace(start, 0);
    backward.index.emplace(goal, 0);

    std::priority_queue<
        PairEntry, std::vector<PairEntry>, PairCompare
    > frontier_pairs;
    std::size_t order = 0;
    std::optional<Cost> incumbent;
    std::size_t meeting_forward = 0;
    std::size_t meeting_backward = 0;

    const auto push_pair = [&](std::size_t forward_index,
                               std::size_t backward_index){
        const Node& forward_node = forward.nodes[forward_index];
        const Node& backward_node = backward.nodes[backward_index];
        const Priority estimate = static_cast<Priority>(std::invoke(
            front_to_front_heuristic,
            forward_node.state,
            backward_node.state
        ));
        if(estimate < Priority{})[[unlikely]]{
            throw std::invalid_argument(
                "front_to_front_bidirectional_search requires "
                "non-negative heuristic estimates"
            );
        }
        frontier_pairs.push(PairEntry{
            static_cast<Priority>(forward_node.distance)
                + estimate
                + static_cast<Priority>(backward_node.distance),
            order++,
            forward_index,
            backward_index,
            forward_node.version,
            backward_node.version
        });
    };
    const auto push_pairs_for = [&](std::size_t node_index, bool is_forward){
        if(is_forward){
            for(std::size_t i = 0; i < backward.nodes.size(); ++i){
                if(backward.nodes[i].open) push_pair(node_index, i);
            }
        }else{
            for(std::size_t i = 0; i < forward.nodes.size(); ++i){
                if(forward.nodes[i].open) push_pair(i, node_index);
            }
        }
    };
    const auto pair_is_current = [&](const PairEntry& entry){
        const Node& forward_node = forward.nodes[entry.forward_node];
        const Node& backward_node = backward.nodes[entry.backward_node];
        return forward_node.open
            && backward_node.open
            && forward_node.version == entry.forward_version
            && backward_node.version == entry.backward_version;
    };
    const auto discard_stale_pairs = [&]{
        while(!frontier_pairs.empty()
              && !pair_is_current(frontier_pairs.top())){
            frontier_pairs.pop();
        }
    };
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

    push_pair(0, 0);
    bool prefer_forward = true;
    while(true){
        discard_stale_pairs();
        if(frontier_pairs.empty()) break;
        if(incumbent
           && !(frontier_pairs.top().lower_bound
                < static_cast<Priority>(*incumbent))){
            break;
        }

        const PairEntry best_pair = frontier_pairs.top();
        frontier_pairs.pop();
        const bool expand_forward = prefer_forward;
        prefer_forward = !prefer_forward;

        const auto expand = [&](Side& own, auto& neighbors, bool is_forward,
                                std::size_t current_index){
            Node& selected = own.nodes[current_index];
            selected.open = false;
            ++selected.version;
            const State current_state = selected.state;
            const Cost current_distance = selected.distance;
            ++result.expanded;
            update_meeting(current_index, is_forward);

            auto adjacent = std::invoke(neighbors, current_state);
            for(const auto& edge : adjacent){
                const Cost edge_cost = static_cast<Cost>(edge.second);
                if(edge_cost < Cost{})[[unlikely]]{
                    throw std::invalid_argument(
                        "front_to_front_bidirectional_search "
                        "does not accept negative edges"
                    );
                }
                const Cost candidate = current_distance + edge_cost;
                const State& next_state = edge.first;
                auto iterator = own.index.find(next_state);
                std::size_t next_index;
                if(iterator == own.index.end()){
                    next_index = own.nodes.size();
                    own.nodes.push_back(Node{
                        next_state, candidate, current_index, 0, true, true
                    });
                    own.index.emplace(own.nodes.back().state, next_index);
                }else{
                    next_index = iterator->second;
                    Node& next = own.nodes[next_index];
                    if(!(candidate < next.distance)) continue;
                    next.distance = candidate;
                    next.parent = current_index;
                    next.has_parent = true;
                    next.open = true;
                    ++next.version;
                }
                update_meeting(next_index, is_forward);
                push_pairs_for(next_index, is_forward);
            }
        };

        if(expand_forward){
            expand(
                forward, forward_neighbors, true, best_pair.forward_node
            );
        }else{
            expand(
                backward, backward_neighbors, false, best_pair.backward_node
            );
        }
    }

    if(!incumbent) return result;
    result.cost = incumbent;
    result.path.clear();
    std::size_t forward_path_index = meeting_forward;
    while(true){
        result.path.push_back(forward.nodes[forward_path_index].state);
        if(!forward.nodes[forward_path_index].has_parent) break;
        forward_path_index = forward.nodes[forward_path_index].parent;
    }
    std::reverse(result.path.begin(), result.path.end());
    std::size_t index = meeting_backward;
    while(backward.nodes[index].has_parent){
        index = backward.nodes[index].parent;
        result.path.push_back(backward.nodes[index].state);
    }
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_FRONT_TO_FRONT_BIDIRECTIONAL_SEARCH_HPP_INCLUDED
