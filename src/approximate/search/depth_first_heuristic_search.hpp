#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_DEPTH_FIRST_HEURISTIC_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_DEPTH_FIRST_HEURISTIC_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include "heuristic_search.hpp"

template<class State, class Goal, class Neighbors, class Heuristic, class Weight,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto weighted_ida_star_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    Weight weight,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using Result = HeuristicSearchResult<State, Cost>;

    const long double search_weight = static_cast<long double>(weight);
    if(!(search_weight >= 1.0L) || !std::isfinite(search_weight))[[unlikely]]{
        throw std::invalid_argument(
            "weighted_ida_star_search requires a finite weight >= 1"
        );
    }
    const auto estimate = [&](const State& state){
        const long double value = static_cast<long double>(
            std::invoke(heuristic, state)
        );
        if(value < 0.0L || !std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument(
                "weighted_ida_star_search requires finite non-negative heuristics"
            );
        }
        return value;
    };

    Result result;
    std::vector<State> path{start};
    std::unordered_set<State, Hash, Equal> on_path(0, std::move(hash), std::move(equal));
    on_path.insert(start);
    long double bound = search_weight * estimate(start);
    const long double infinity = std::numeric_limits<long double>::infinity();
    bool found = false;

    const auto visit = [&](auto&& self, const State& state, Cost distance,
                           long double current_bound) -> long double {
        const long double score = static_cast<long double>(distance)
                                + search_weight * estimate(state);
        if(score > current_bound) return score;
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, state))){
            result.cost = distance;
            result.path = path;
            found = true;
            return score;
        }

        long double next_bound = infinity;
        auto&& adjacent = std::invoke(neighbors, state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument(
                    "weighted_ida_star_search does not accept negative edges"
                );
            }
            const State& next = edge.first;
            if(on_path.find(next) != on_path.end()) continue;
            path.push_back(next);
            on_path.insert(next);
            const long double candidate = self(
                self, next, static_cast<Cost>(distance + edge_cost), current_bound
            );
            on_path.erase(next);
            path.pop_back();
            if(found) return candidate;
            if(candidate < next_bound) next_bound = candidate;
        }
        return next_bound;
    };

    while(true){
        const long double next_bound = visit(visit, start, Cost{}, bound);
        if(found) return result;
        if(!std::isfinite(next_bound)) return result;
        bound = next_bound;
    }
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto recursive_best_first_search(
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
    using Result = HeuristicSearchResult<State, Cost>;

    struct Successor{
        State state;
        Cost distance;
        long double score;
    };

    const auto estimate = [&](const State& state){
        const long double value = static_cast<long double>(
            std::invoke(heuristic, state)
        );
        if(value < 0.0L || !std::isfinite(value))[[unlikely]]{
            throw std::invalid_argument(
                "recursive_best_first_search requires finite non-negative heuristics"
            );
        }
        return value;
    };

    Result result;
    std::vector<State> path{start};
    std::unordered_set<State, Hash, Equal> on_path(0, std::move(hash), std::move(equal));
    on_path.insert(start);
    const long double infinity = std::numeric_limits<long double>::infinity();
    bool found = false;

    const auto visit = [&](auto&& self, const State& state, Cost distance,
                           long double node_score,
                           long double limit) -> long double {
        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, state))){
            result.cost = distance;
            result.path = path;
            found = true;
            return node_score;
        }

        std::vector<Successor> successors;
        auto&& adjacent = std::invoke(neighbors, state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument(
                    "recursive_best_first_search does not accept negative edges"
                );
            }
            const State& next = edge.first;
            if(on_path.find(next) != on_path.end()) continue;
            const Cost candidate = static_cast<Cost>(distance + edge_cost);
            successors.push_back(Successor{
                next,
                candidate,
                std::max(
                    node_score,
                    static_cast<long double>(candidate) + estimate(next)
                )
            });
        }
        if(successors.empty()) return infinity;

        while(true){
            std::sort(
                successors.begin(), successors.end(),
                [](const Successor& lhs, const Successor& rhs){
                    return lhs.score < rhs.score;
                }
            );
            Successor& best = successors.front();
            if(!std::isfinite(best.score) || best.score > limit) return best.score;
            const long double alternative = successors.size() == 1
                ? infinity : successors[1].score;
            path.push_back(best.state);
            on_path.insert(best.state);
            best.score = self(
                self, best.state, best.distance, best.score,
                std::min(limit, alternative)
            );
            on_path.erase(best.state);
            path.pop_back();
            if(found) return best.score;
        }
    };

    const long double root_score = estimate(start);
    static_cast<void>(visit(visit, start, Cost{}, root_score, infinity));
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_DEPTH_FIRST_HEURISTIC_SEARCH_HPP_INCLUDED
