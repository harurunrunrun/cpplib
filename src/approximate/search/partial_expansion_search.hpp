#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_PARTIAL_EXPANSION_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_PARTIAL_EXPANSION_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "heuristic_search.hpp"

namespace partial_expansion_search_internal{

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

template<class State, class Cost>
struct QueueEntry{
    long double score;
    std::size_t order;
    std::size_t node;
};

template<class State, class Cost>
struct QueueCompare{
    bool operator()(
        const QueueEntry<State, Cost>& lhs,
        const QueueEntry<State, Cost>& rhs
    ) const{
        if(rhs.score < lhs.score) return true;
        if(lhs.score < rhs.score) return false;
        return rhs.order < lhs.order;
    }
};

}  // namespace partial_expansion_search_internal

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto fringe_search(
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
    using Node = partial_expansion_search_internal::Node<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;

    const auto estimate = [&](const State& state){
        const long double value = static_cast<long double>(
            std::invoke(heuristic, state)
        );
        if(value < 0.0L || !std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument(
                "fringe_search requires finite non-negative heuristics"
            );
        }
        return value;
    };
    const long double infinity = std::numeric_limits<long double>::infinity();
    long double threshold = estimate(start);
    std::vector<Node> nodes{{start, Cost{}, 0, false}};
    std::vector<std::size_t> current{0};
    std::vector<std::size_t> later;
    std::unordered_map<State, std::size_t, Hash, Equal> best(
        0, std::move(hash), std::move(equal)
    );
    best.emplace(start, 0);
    Result result;

    while(!current.empty()){
        long double next_threshold = infinity;
        while(!current.empty()){
            const std::size_t index = current.back();
            current.pop_back();
            const Node node = nodes[index];
            const auto known = best.find(node.state);
            if(known == best.end() || known->second != index) continue;
            const long double score = static_cast<long double>(node.distance)
                                    + estimate(node.state);
            if(score > threshold){
                later.push_back(index);
                if(score < next_threshold) next_threshold = score;
                continue;
            }
            ++result.expanded;
            if(static_cast<bool>(std::invoke(goal, node.state))){
                result.cost = node.distance;
                result.path = partial_expansion_search_internal::restore_path(
                    nodes, index
                );
                return result;
            }
            auto&& adjacent = std::invoke(neighbors, node.state);
            for(auto iterator = std::rbegin(adjacent);
                iterator != std::rend(adjacent); ++iterator){
                const Cost edge_cost = static_cast<Cost>(iterator->second);
                if(edge_cost < Cost{})[[unlikely]]{
                    throw std::invalid_argument(
                        "fringe_search does not accept negative edges"
                    );
                }
                const Cost candidate = static_cast<Cost>(
                    node.distance + edge_cost
                );
                const State& next = iterator->first;
                const auto previous = best.find(next);
                if(previous != best.end()
                   && !(candidate < nodes[previous->second].distance)){
                    continue;
                }
                const std::size_t next_index = nodes.size();
                nodes.push_back(Node{next, candidate, index, true});
                best.insert_or_assign(nodes.back().state, next_index);
                current.push_back(next_index);
            }
        }
        if(later.empty() || !std::isfinite(next_threshold)) break;
        current.swap(later);
        later.clear();
        threshold = next_threshold;
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto partial_expansion_a_star_search(
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
    using BaseNode = partial_expansion_search_internal::Node<State, Cost>;
    using Entry = partial_expansion_search_internal::QueueEntry<State, Cost>;
    using Compare = partial_expansion_search_internal::QueueCompare<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;
    struct Successor{
        State state;
        Cost distance;
        long double score;
    };
    struct Node : BaseNode{
        std::vector<Successor> successors;
        std::size_t next = 0;
        bool initialized = false;
    };

    const auto estimate = [&](const State& state){
        const long double value = static_cast<long double>(
            std::invoke(heuristic, state)
        );
        if(value < 0.0L || !std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument(
                "partial_expansion_a_star_search requires finite "
                "non-negative heuristics"
            );
        }
        return value;
    };
    std::vector<Node> nodes;
    nodes.push_back(Node{{start, Cost{}, 0, false}, {}, 0, false});
    std::unordered_map<State, std::size_t, Hash, Equal> best(
        0, std::move(hash), std::move(equal)
    );
    best.emplace(start, 0);
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    std::size_t order = 0;
    queue.push(Entry{estimate(start), order++, 0});
    Result result;

    while(!queue.empty()){
        const Entry entry = queue.top();
        queue.pop();
        const State current_state = nodes[entry.node].state;
        const auto known = best.find(current_state);
        if(known == best.end() || known->second != entry.node) continue;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current_state))){
            result.cost = nodes[entry.node].distance;
            std::vector<BaseNode> path_nodes;
            path_nodes.reserve(nodes.size());
            for(const Node& node : nodes) path_nodes.push_back(node);
            result.path = partial_expansion_search_internal::restore_path(
                path_nodes, entry.node
            );
            return result;
        }

        if(!nodes[entry.node].initialized){
            const Cost current_distance = nodes[entry.node].distance;
            auto&& adjacent = std::invoke(neighbors, current_state);
            std::vector<Successor> generated;
            for(const auto& edge : adjacent){
                const Cost edge_cost = static_cast<Cost>(edge.second);
                if(edge_cost < Cost{})[[unlikely]]{
                    throw std::invalid_argument(
                        "partial_expansion_a_star_search does not accept "
                        "negative edges"
                    );
                }
                const Cost candidate = static_cast<Cost>(
                    current_distance + edge_cost
                );
                generated.push_back(Successor{
                    edge.first,
                    candidate,
                    static_cast<long double>(candidate) + estimate(edge.first)
                });
            }
            std::sort(
                generated.begin(), generated.end(),
                [](const Successor& lhs, const Successor& rhs){
                    return lhs.score < rhs.score;
                }
            );
            nodes[entry.node].successors = std::move(generated);
            nodes[entry.node].initialized = true;
        }

        bool inserted = false;
        while(nodes[entry.node].next < nodes[entry.node].successors.size()){
            const Successor successor =
                nodes[entry.node].successors[nodes[entry.node].next++];
            const auto previous = best.find(successor.state);
            if(previous != best.end()
               && !(successor.distance < nodes[previous->second].distance)){
                continue;
            }
            const std::size_t child = nodes.size();
            nodes.push_back(Node{{
                successor.state, successor.distance, entry.node, true
            }, {}, 0, false});
            best.insert_or_assign(nodes.back().state, child);
            queue.push(Entry{successor.score, order++, child});
            inserted = true;
            break;
        }
        if(nodes[entry.node].next < nodes[entry.node].successors.size()){
            queue.push(Entry{
                nodes[entry.node].successors[nodes[entry.node].next].score,
                order++,
                entry.node
            });
        }else if(!inserted){
            nodes[entry.node].successors.clear();
        }
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto pea_star_search(
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
    using BaseNode = partial_expansion_search_internal::Node<State, Cost>;
    using Entry = partial_expansion_search_internal::QueueEntry<State, Cost>;
    using Compare = partial_expansion_search_internal::QueueCompare<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;
    struct Node : BaseNode{
        long double expansion_bound;
    };

    const auto estimate = [&](const State& state){
        const long double value = static_cast<long double>(
            std::invoke(heuristic, state)
        );
        if(value < 0.0L || !std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument(
                "pea_star_search requires finite non-negative heuristics"
            );
        }
        return value;
    };
    const long double infinity = std::numeric_limits<long double>::infinity();
    std::vector<Node> nodes;
    nodes.push_back(Node{{start, Cost{}, 0, false}, estimate(start)});
    std::unordered_map<State, std::size_t, Hash, Equal> best(
        0, std::move(hash), std::move(equal)
    );
    best.emplace(start, 0);
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    std::size_t order = 0;
    queue.push(Entry{nodes[0].expansion_bound, order++, 0});
    Result result;

    while(!queue.empty()){
        const Entry entry = queue.top();
        queue.pop();
        const State current_state = nodes[entry.node].state;
        const Cost current_distance = nodes[entry.node].distance;
        const auto known = best.find(current_state);
        if(known == best.end() || known->second != entry.node) continue;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current_state))){
            result.cost = current_distance;
            std::vector<BaseNode> path_nodes;
            path_nodes.reserve(nodes.size());
            for(const Node& node : nodes) path_nodes.push_back(node);
            result.path = partial_expansion_search_internal::restore_path(
                path_nodes, entry.node
            );
            return result;
        }

        long double next_bound = infinity;
        const long double expansion_bound = nodes[entry.node].expansion_bound;
        auto&& adjacent = std::invoke(neighbors, current_state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument(
                    "pea_star_search does not accept negative edges"
                );
            }
            const Cost candidate = static_cast<Cost>(
                current_distance + edge_cost
            );
            const long double score = static_cast<long double>(candidate)
                                    + estimate(edge.first);
            if(score > expansion_bound){
                if(score < next_bound) next_bound = score;
                continue;
            }
            const auto previous = best.find(edge.first);
            if(previous != best.end()
               && !(candidate < nodes[previous->second].distance)){
                continue;
            }
            const std::size_t child = nodes.size();
            nodes.push_back(Node{{
                edge.first, candidate, entry.node, true
            }, score});
            best.insert_or_assign(nodes.back().state, child);
            queue.push(Entry{score, order++, child});
        }
        if(std::isfinite(next_bound)){
            nodes[entry.node].expansion_bound = next_bound;
            queue.push(Entry{next_bound, order++, entry.node});
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_PARTIAL_EXPANSION_SEARCH_HPP_INCLUDED
