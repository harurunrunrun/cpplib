#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "heuristic_search.hpp"

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto beam_search(
    const State& start,
    Goal goal,
    Neighbors neighbors,
    Heuristic heuristic,
    std::size_t beam_width,
    std::size_t maximum_depth,
    Hash hash = {},
    Equal equal = {}
){
    using Range = std::invoke_result_t<Neighbors&, const State&>;
    using Edge = heuristic_search_internal::range_value_t<Range>;
    using Cost = std::decay_t<decltype(std::declval<Edge>().second)>;
    using HeuristicValue = std::decay_t<std::invoke_result_t<Heuristic&, const State&>>;
    using Priority = std::common_type_t<Cost, HeuristicValue>;
    using Node = heuristic_search_internal::Node<State, Cost>;
    using Result = HeuristicSearchResult<State, Cost>;

    struct Candidate{
        Priority priority;
        Cost distance;
        std::size_t order;
        std::size_t node;
    };

    if(beam_width == 0)[[unlikely]]{
        throw std::invalid_argument("beam_search requires a positive beam width");
    }

    std::vector<Node> nodes;
    std::unordered_set<State, Hash, Equal> expanded_states(0, hash, equal);
    nodes.push_back(Node{start, Cost{}, 0, false});
    std::vector<std::size_t> beam{0};
    std::size_t order = 0;
    Result result;

    for(std::size_t depth = 0; depth <= maximum_depth; ++depth){
        for(const std::size_t index : beam){
            if(static_cast<bool>(std::invoke(goal, nodes[index].state))){
                result.cost = nodes[index].distance;
                result.path = heuristic_search_internal::restore_path(nodes, index);
                return result;
            }
        }
        if(depth == maximum_depth) break;

        std::vector<Candidate> candidates;
        std::unordered_map<State, std::size_t, Hash, Equal> candidate_index(
            0, hash, equal
        );
        for(const std::size_t index : beam){
            if(!expanded_states.insert(nodes[index].state).second) continue;
            ++result.expanded;
            const State current_state = nodes[index].state;
            const Cost current_distance = nodes[index].distance;
            auto adjacent = std::invoke(neighbors, current_state);
            for(const auto& edge : adjacent){
                const Cost edge_cost = static_cast<Cost>(edge.second);
                if(edge_cost < Cost{})[[unlikely]]{
                    throw std::invalid_argument("beam_search does not accept negative edges");
                }
                const Cost candidate_distance = current_distance + edge_cost;
                const State& next_state = edge.first;
                if(expanded_states.find(next_state) != expanded_states.end()) continue;
                const auto iterator = candidate_index.find(next_state);
                if(iterator != candidate_index.end()){
                    Candidate& old = candidates[iterator->second];
                    if(!(candidate_distance < old.distance)) continue;
                    old.distance = candidate_distance;
                    old.priority = static_cast<Priority>(candidate_distance)
                        + static_cast<Priority>(std::invoke(heuristic, next_state));
                    nodes[old.node].distance = candidate_distance;
                    nodes[old.node].parent = index;
                    continue;
                }

                const std::size_t next_index = nodes.size();
                nodes.push_back(Node{next_state, candidate_distance, index, true});
                const std::size_t position = candidates.size();
                candidates.push_back(Candidate{
                    static_cast<Priority>(candidate_distance)
                        + static_cast<Priority>(std::invoke(heuristic, next_state)),
                    candidate_distance, order++, next_index
                });
                candidate_index.emplace(nodes.back().state, position);
            }
        }

        std::sort(candidates.begin(), candidates.end(), [](const Candidate& lhs, const Candidate& rhs){
            if(lhs.priority < rhs.priority) return true;
            if(rhs.priority < lhs.priority) return false;
            return lhs.order < rhs.order;
        });
        beam.clear();
        beam.reserve(std::min(beam_width, candidates.size()));
        for(const Candidate& candidate : candidates){
            beam.push_back(candidate.node);
            if(beam.size() == beam_width) break;
        }
        if(beam.empty()) break;
    }
    return result;
}

template<class State, class Goal, class Neighbors, class Heuristic,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto ida_star_search(
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

    Result result;
    Cost bound = static_cast<Cost>(std::invoke(heuristic, start));
    if(bound < Cost{})[[unlikely]]{
        throw std::invalid_argument("ida_star_search requires a non-negative heuristic");
    }

    std::vector<State> path{start};
    std::unordered_set<State, Hash, Equal> on_path(0, hash, equal);
    on_path.insert(start);
    bool found = false;

    const auto dfs = [&](auto&& self, const State& state, Cost distance,
                         Cost current_bound) -> std::optional<Cost>{
        const Cost estimate = static_cast<Cost>(std::invoke(heuristic, state));
        if(estimate < Cost{})[[unlikely]]{
            throw std::invalid_argument("ida_star_search requires a non-negative heuristic");
        }
        const Cost lower_bound = distance + estimate;
        if(current_bound < lower_bound) return lower_bound;

        ++result.expanded;
        if(static_cast<bool>(std::invoke(goal, state))){
            result.cost = distance;
            result.path = path;
            found = true;
            return std::nullopt;
        }

        std::optional<Cost> next_bound;
        auto adjacent = std::invoke(neighbors, state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument("ida_star_search does not accept negative edges");
            }
            const State& next_state = edge.first;
            if(on_path.find(next_state) != on_path.end()) continue;
            on_path.insert(next_state);
            path.push_back(next_state);
            const std::optional<Cost> candidate = self(
                self, next_state, distance + edge_cost, current_bound
            );
            path.pop_back();
            on_path.erase(next_state);
            if(found) return std::nullopt;
            if(candidate && (!next_bound || *candidate < *next_bound)) next_bound = candidate;
        }
        return next_bound;
    };

    while(true){
        const std::optional<Cost> next_bound = dfs(dfs, start, Cost{}, bound);
        if(found || !next_bound) return result;
        if(!((bound) < *next_bound))[[unlikely]]{
            return result;
        }
        bound = *next_bound;
    }
}
