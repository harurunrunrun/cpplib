#ifndef CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_HILL_CLIMBING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_HILL_CLIMBING_HPP_INCLUDED

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "search_result.hpp"

namespace approximate::metaheuristic {

template<class State, class Neighborhood, class Evaluate,
         class Compare = std::less<>>
auto best_improvement_hill_climbing(
    State initial_state,
    Neighborhood neighborhood,
    Evaluate evaluate,
    std::size_t maximum_iterations,
    Compare compare = {}
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score initial_score = std::invoke(evaluate, initial_state);
    SearchResult<State, Score> result{
        std::move(initial_state), std::move(initial_score), 0, 1, 0
    };

    for(std::size_t iteration = 0;
        iteration < maximum_iterations;
        ++iteration){
        result.iterations = iteration + 1;
        auto candidates = std::invoke(neighborhood, result.state);
        bool found = false;
        State best_state = result.state;
        Score best_score = result.score;
        for(auto&& value: candidates){
            State candidate = std::forward<decltype(value)>(value);
            Score candidate_score = std::invoke(evaluate, candidate);
            ++result.evaluations;
            if(std::invoke(compare, candidate_score, best_score)){
                best_state = std::move(candidate);
                best_score = std::move(candidate_score);
                found = true;
            }
        }
        if(!found) break;
        result.state = std::move(best_state);
        result.score = std::move(best_score);
        ++result.accepted_moves;
    }
    return result;
}

template<class State, class Neighborhood, class Evaluate,
         class Compare = std::less<>>
auto first_improvement_hill_climbing(
    State initial_state,
    Neighborhood neighborhood,
    Evaluate evaluate,
    std::size_t maximum_iterations,
    Compare compare = {}
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score initial_score = std::invoke(evaluate, initial_state);
    SearchResult<State, Score> result{
        std::move(initial_state), std::move(initial_score), 0, 1, 0
    };

    for(std::size_t iteration = 0;
        iteration < maximum_iterations;
        ++iteration){
        result.iterations = iteration + 1;
        auto candidates = std::invoke(neighborhood, result.state);
        bool found = false;
        for(auto&& value: candidates){
            State candidate = std::forward<decltype(value)>(value);
            Score candidate_score = std::invoke(evaluate, candidate);
            ++result.evaluations;
            if(!std::invoke(compare, candidate_score, result.score)) continue;
            result.state = std::move(candidate);
            result.score = std::move(candidate_score);
            ++result.accepted_moves;
            found = true;
            break;
        }
        if(!found) break;
    }
    return result;
}

template<class State, class RandomNeighbor, class Evaluate, class URBG,
         class Compare = std::less<>>
auto stochastic_hill_climbing(
    State initial_state,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    URBG& random,
    Compare compare = {}
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score initial_score = std::invoke(evaluate, initial_state);
    SearchResult<State, Score> result{
        std::move(initial_state), std::move(initial_score), iterations, 1, 0
    };
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        State candidate = std::invoke(random_neighbor, result.state, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(!std::invoke(compare, candidate_score, result.score)) continue;
        result.state = std::move(candidate);
        result.score = std::move(candidate_score);
        ++result.accepted_moves;
    }
    return result;
}

template<class Generator, class Evaluate, class URBG,
         class Compare = std::less<>>
auto random_search(
    Generator generator,
    Evaluate evaluate,
    std::size_t samples,
    URBG& random,
    Compare compare = {}
){
    if(samples == 0)[[unlikely]]{
        throw std::invalid_argument("random_search requires a positive sample count");
    }
    using State = std::decay_t<std::invoke_result_t<Generator&, URBG&>>;
    State state = std::invoke(generator, random);
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score initial_score = std::invoke(evaluate, state);
    SearchResult<State, Score> result{
        std::move(state), std::move(initial_score), samples, 1, 0
    };
    for(std::size_t sample = 1; sample < samples; ++sample){
        State candidate = std::invoke(generator, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(!std::invoke(compare, candidate_score, result.score)) continue;
        result.state = std::move(candidate);
        result.score = std::move(candidate_score);
        ++result.accepted_moves;
    }
    return result;
}

template<class Generator, class Neighborhood, class Evaluate, class URBG,
         class Compare = std::less<>>
auto random_restart_hill_climbing(
    Generator generator,
    Neighborhood neighborhood,
    Evaluate evaluate,
    std::size_t restarts,
    std::size_t maximum_iterations_per_restart,
    URBG& random,
    Compare compare = {}
){
    if(restarts == 0)[[unlikely]]{
        throw std::invalid_argument(
            "random_restart_hill_climbing requires a positive restart count"
        );
    }
    auto best = best_improvement_hill_climbing(
        std::invoke(generator, random), neighborhood, evaluate,
        maximum_iterations_per_restart, compare
    );
    for(std::size_t restart = 1; restart < restarts; ++restart){
        auto candidate = best_improvement_hill_climbing(
            std::invoke(generator, random), neighborhood, evaluate,
            maximum_iterations_per_restart, compare
        );
        best.iterations += candidate.iterations;
        best.evaluations += candidate.evaluations;
        best.accepted_moves += candidate.accepted_moves;
        if(std::invoke(compare, candidate.score, best.score)){
            best.state = std::move(candidate.state);
            best.score = std::move(candidate.score);
        }
    }
    return best;
}

}  // namespace approximate::metaheuristic

#endif  // CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_HILL_CLIMBING_HPP_INCLUDED
