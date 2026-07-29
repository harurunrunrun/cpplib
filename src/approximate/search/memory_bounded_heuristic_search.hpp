#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_MEMORY_BOUNDED_HEURISTIC_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_MEMORY_BOUNDED_HEURISTIC_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "heuristic_search.hpp"

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto simplified_memory_bounded_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t max_frontier_size,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Result = HeuristicSearchResult<State, Cost>;
    struct Node{
        State state;
        Cost distance;
        long double score;
        std::vector<State> path;
        std::size_t order;
    };

    if(max_frontier_size == 0)[[unlikely]]{
        throw std::invalid_argument(
            "simplified_memory_bounded_a_star_search requires a positive frontier size"
        );
    }
    const auto estimate = [&](const State& state){
        const long double value = static_cast<long double>(
            std::invoke(heuristic, state)
        );
        if(value < 0.0L || !std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument(
                "simplified_memory_bounded_a_star_search requires finite "
                "non-negative heuristics"
            );
        }
        return value;
    };

    Result result;
    std::vector<Node> frontier;
    frontier.push_back(Node{start, Cost{}, estimate(start), {start}, 0});
    std::unordered_map<State, Cost, Hash, Equal> best_distance(
        0, std::move(hash), std::move(equal)
    );
    best_distance.emplace(start, Cost{});
    std::size_t order = 1;

    while(!frontier.empty()){
        const auto best_iterator = std::min_element(
            frontier.begin(), frontier.end(),
            [](const Node& lhs, const Node& rhs){
                if(lhs.score != rhs.score) return lhs.score < rhs.score;
                if(lhs.path.size() != rhs.path.size()){
                    return lhs.path.size() > rhs.path.size();
                }
                return lhs.order < rhs.order;
            }
        );
        Node current = std::move(*best_iterator);
        frontier.erase(best_iterator);
        const auto known = best_distance.find(current.state);
        if(known != best_distance.end() && known->second < current.distance) continue;

        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current.state))){
            result.cost = current.distance;
            result.path = std::move(current.path);
            return result;
        }
        auto&& adjacent = std::invoke(neighbors, current.state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument(
                    "simplified_memory_bounded_a_star_search does not accept "
                    "negative edges"
                );
            }
            const State& next = edge.first;
            if(std::find_if(
                   current.path.begin(), current.path.end(),
                   [&](const State& state){
                       return static_cast<bool>(std::invoke(equal, state, next));
                   }
               ) != current.path.end()){
                continue;
            }
            const Cost candidate = static_cast<Cost>(current.distance + edge_cost);
            const auto previous = best_distance.find(next);
            if(previous != best_distance.end() && !(candidate < previous->second)){
                continue;
            }
            best_distance.insert_or_assign(next, candidate);
            std::vector<State> next_path = current.path;
            next_path.push_back(next);
            frontier.push_back(Node{
                next,
                candidate,
                std::max(
                    current.score,
                    static_cast<long double>(candidate) + estimate(next)
                ),
                std::move(next_path),
                order++
            });
        }
        while(frontier.size() > max_frontier_size){
            const auto worst_iterator = std::max_element(
                frontier.begin(), frontier.end(),
                [](const Node& lhs, const Node& rhs){
                    if(lhs.score != rhs.score) return lhs.score < rhs.score;
                    if(lhs.path.size() != rhs.path.size()){
                        return lhs.path.size() > rhs.path.size();
                    }
                    return lhs.order > rhs.order;
                }
            );
            frontier.erase(worst_iterator);
        }
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto memory_bounded_a_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t max_frontier_size,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Result = HeuristicSearchResult<State, Cost>;
    struct Node{
        State state;
        Cost distance;
        long double score;
        std::vector<State> path;
        std::size_t order;
    };

    if(max_frontier_size == 0)[[unlikely]]{
        throw std::invalid_argument(
            "memory_bounded_a_star_search requires a positive frontier size"
        );
    }
    const auto estimate = [&](const State& state){
        const long double value = static_cast<long double>(
            std::invoke(heuristic, state)
        );
        if(value < 0.0L || !std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument(
                "memory_bounded_a_star_search requires finite non-negative heuristics"
            );
        }
        return value;
    };

    Result result;
    std::vector<Node> frontier;
    frontier.push_back(Node{start, Cost{}, estimate(start), {start}, 0});
    std::unordered_map<State, Cost, Hash, Equal> best_distance(
        0, std::move(hash), std::move(equal)
    );
    best_distance.emplace(start, Cost{});
    std::size_t order = 1;

    while(!frontier.empty()){
        std::size_t selected = 0;
        for(std::size_t i = 1; i < frontier.size(); ++i){
            if(frontier[i].score < frontier[selected].score
               || (frontier[i].score == frontier[selected].score
                   && frontier[i].order < frontier[selected].order)){
                selected = i;
            }
        }
        Node current = std::move(frontier[selected]);
        frontier.erase(frontier.begin() + static_cast<std::ptrdiff_t>(selected));
        const auto known = best_distance.find(current.state);
        if(known != best_distance.end() && known->second < current.distance) continue;

        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, current.state))){
            result.cost = current.distance;
            result.path = std::move(current.path);
            return result;
        }
        auto&& adjacent = std::invoke(neighbors, current.state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument(
                    "memory_bounded_a_star_search does not accept negative edges"
                );
            }
            const State& next = edge.first;
            if(std::find_if(
                   current.path.begin(), current.path.end(),
                   [&](const State& state){
                       return static_cast<bool>(std::invoke(equal, state, next));
                   }
               ) != current.path.end()){
                continue;
            }
            const Cost candidate = static_cast<Cost>(current.distance + edge_cost);
            const auto previous = best_distance.find(next);
            if(previous != best_distance.end() && !(candidate < previous->second)){
                continue;
            }
            best_distance.insert_or_assign(next, candidate);
            std::vector<State> next_path = current.path;
            next_path.push_back(next);
            frontier.push_back(Node{
                next,
                candidate,
                static_cast<long double>(candidate) + estimate(next),
                std::move(next_path),
                order++
            });
            if(frontier.size() <= max_frontier_size) continue;
            std::size_t worst = 0;
            for(std::size_t i = 1; i < frontier.size(); ++i){
                if(frontier[worst].score < frontier[i].score
                   || (frontier[worst].score == frontier[i].score
                       && frontier[worst].order < frontier[i].order)){
                    worst = i;
                }
            }
            frontier.erase(frontier.begin() + static_cast<std::ptrdiff_t>(worst));
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_MEMORY_BOUNDED_HEURISTIC_SEARCH_HPP_INCLUDED
