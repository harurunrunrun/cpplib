#ifndef CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_TABU_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_TABU_SEARCH_HPP_INCLUDED

#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "search_result.hpp"

namespace approximate::metaheuristic {

template<class State, class Neighborhood, class Evaluate, class KeyFunction,
         class Hash = std::hash<std::decay_t<
             std::invoke_result_t<KeyFunction&, const State&>
         >>,
         class Compare = std::less<>>
auto tabu_search(
    State initial_state,
    Neighborhood neighborhood,
    Evaluate evaluate,
    KeyFunction key,
    std::size_t iterations,
    std::size_t tabu_tenure,
    Hash hash = {},
    Compare compare = {}
){
    if(tabu_tenure == 0)[[unlikely]]{
        throw std::invalid_argument("tabu_search requires positive tenure");
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    using Key = std::decay_t<std::invoke_result_t<KeyFunction&, const State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, 0, 1, 0
    };
    std::unordered_map<Key, std::size_t, Hash> forbidden(0, std::move(hash));

    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto candidates = std::invoke(neighborhood, current);
        std::optional<std::pair<State, Score>> selected;
        for(auto&& value: candidates){
            State candidate = std::forward<decltype(value)>(value);
            Score candidate_score = std::invoke(evaluate, candidate);
            ++result.evaluations;
            const Key candidate_key = std::invoke(key, candidate);
            const auto found = forbidden.find(candidate_key);
            const bool is_tabu = found != forbidden.end()
                && found->second > iteration;
            const bool aspiration =
                std::invoke(compare, candidate_score, result.score);
            if(is_tabu && !aspiration) continue;
            if(!selected.has_value()
                || std::invoke(compare, candidate_score, selected->second)){
                selected.emplace(
                    std::move(candidate), std::move(candidate_score)
                );
            }
        }
        result.iterations = iteration + 1;
        if(!selected.has_value()) break;
        const Key previous_key = std::invoke(key, std::as_const(current));
        current = std::move(selected->first);
        current_score = std::move(selected->second);
        forbidden[previous_key] = iteration + tabu_tenure + 1;
        ++result.accepted_moves;
        if(std::invoke(compare, current_score, result.score)){
            result.state = current;
            result.score = current_score;
        }
    }
    return result;
}

}  // namespace approximate::metaheuristic

#endif  // CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_TABU_SEARCH_HPP_INCLUDED
