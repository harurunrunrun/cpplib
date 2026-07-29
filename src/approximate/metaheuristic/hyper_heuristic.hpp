#ifndef CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_HYPER_HEURISTIC_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_HYPER_HEURISTIC_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <iterator>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "search_result.hpp"

namespace approximate::metaheuristic {

template<class State, class Heuristics, class Evaluate, class URBG,
         class Compare = std::less<>>
auto hyper_heuristic(
    State initial_state,
    const Heuristics& heuristics,
    Evaluate evaluate,
    std::size_t iterations,
    URBG& random,
    Compare compare = {}
){
    const auto begin = std::begin(heuristics);
    const std::size_t count = static_cast<std::size_t>(
        std::distance(begin, std::end(heuristics))
    );
    if(count == 0)[[unlikely]]{
        throw std::invalid_argument(
            "hyper_heuristic requires at least one low-level heuristic"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    std::uniform_int_distribution<std::size_t> select(0, count - 1);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto heuristic = begin;
        std::advance(
            heuristic,
            static_cast<typename std::iterator_traits<
                decltype(heuristic)
            >::difference_type>(select(random))
        );
        State candidate = std::invoke(*heuristic, current, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(!std::invoke(compare, candidate_score, current_score)) continue;
        current = std::move(candidate);
        current_score = std::move(candidate_score);
        ++result.accepted_moves;
        if(std::invoke(compare, current_score, result.score)){
            result.state = current;
            result.score = current_score;
        }
    }
    return result;
}

template<class State, class Heuristics, class Evaluate, class URBG,
         class Compare = std::less<>>
auto choice_function_hyper_heuristic(
    State initial_state,
    const Heuristics& heuristics,
    Evaluate evaluate,
    std::size_t iterations,
    long double reaction_factor,
    long double recency_weight,
    long double transition_weight,
    URBG& random,
    Compare compare = {}
){
    const auto begin = std::begin(heuristics);
    const std::size_t count = static_cast<std::size_t>(
        std::distance(begin, std::end(heuristics))
    );
    if(count == 0 || !(reaction_factor > 0.0L
                       && reaction_factor <= 1.0L)
       || recency_weight < 0.0L
       || transition_weight < 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "choice_function_hyper_heuristic has invalid parameters"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    std::vector<long double> rewards(count);
    std::vector<std::size_t> last_used(count);
    std::vector<std::vector<long double>> transition(
        count, std::vector<long double>(count)
    );
    std::size_t previous = 0;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        std::size_t selected = 0;
        long double selected_value =
            rewards[0]
            + recency_weight
                * static_cast<long double>(iteration - last_used[0])
            + transition_weight * transition[previous][0];
        for(std::size_t i = 1; i < count; ++i){
            const long double value =
                rewards[i]
                + recency_weight
                    * static_cast<long double>(iteration - last_used[i])
                + transition_weight * transition[previous][i];
            if(selected_value < value){
                selected = i;
                selected_value = value;
            }
        }
        auto heuristic = begin;
        std::advance(
            heuristic,
            static_cast<typename std::iterator_traits<
                decltype(heuristic)
            >::difference_type>(selected)
        );
        State candidate = std::invoke(*heuristic, current, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        const bool improved =
            std::invoke(compare, candidate_score, current_score);
        const long double reward = improved ? 1.0L : 0.0L;
        rewards[selected] =
            (1.0L - reaction_factor) * rewards[selected]
            + reaction_factor * reward;
        transition[previous][selected] =
            (1.0L - reaction_factor)
                * transition[previous][selected]
            + reaction_factor * reward;
        last_used[selected] = iteration + 1;
        previous = selected;
        if(!improved) continue;
        current = std::move(candidate);
        current_score = std::move(candidate_score);
        ++result.accepted_moves;
        if(std::invoke(compare, current_score, result.score)){
            result.state = current;
            result.score = current_score;
        }
    }
    return result;
}

}  // namespace approximate::metaheuristic

#endif  // CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_HYPER_HEURISTIC_HPP_INCLUDED
