#ifndef CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ITERATED_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ITERATED_SEARCH_HPP_INCLUDED

#include <functional>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "search_result.hpp"

namespace approximate::metaheuristic {

template<class State, class LocalImprove, class Perturb, class Evaluate,
         class URBG, class Compare = std::less<>>
auto iterated_local_search(
    State initial_state,
    LocalImprove local_improve,
    Perturb perturb,
    Evaluate evaluate,
    std::size_t iterations,
    URBG& random,
    Compare compare = {}
){
    State current = std::invoke(local_improve, std::move(initial_state));
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        State candidate = std::invoke(perturb, current, random);
        candidate = std::invoke(local_improve, std::move(candidate));
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

template<class State, class Neighborhoods, class Evaluate,
         class Compare = std::less<>>
auto variable_neighborhood_descent(
    State initial_state,
    const Neighborhoods& neighborhoods,
    Evaluate evaluate,
    std::size_t maximum_moves,
    Compare compare = {}
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score initial_score = std::invoke(evaluate, initial_state);
    SearchResult<State, Score> result{
        std::move(initial_state), std::move(initial_score), 0, 1, 0
    };
    const auto begin = std::begin(neighborhoods);
    const auto end = std::end(neighborhoods);
    std::size_t neighborhood_index = 0;
    while(neighborhood_index
            < static_cast<std::size_t>(std::distance(begin, end))
        && result.accepted_moves < maximum_moves){
        auto iterator = begin;
        std::advance(
            iterator,
            static_cast<typename std::iterator_traits<decltype(iterator)>::difference_type>(
                neighborhood_index
            )
        );
        auto candidates = std::invoke(*iterator, result.state);
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
        ++result.iterations;
        if(found){
            result.state = std::move(best_state);
            result.score = std::move(best_score);
            ++result.accepted_moves;
            neighborhood_index = 0;
        }else{
            ++neighborhood_index;
        }
    }
    return result;
}

template<class State, class Shakings, class LocalImprove, class Evaluate,
         class URBG, class Compare = std::less<>>
auto variable_neighborhood_search(
    State initial_state,
    const Shakings& shakings,
    LocalImprove local_improve,
    Evaluate evaluate,
    std::size_t iterations,
    URBG& random,
    Compare compare = {}
){
    State current = std::invoke(local_improve, std::move(initial_state));
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, 0, 1, 0
    };
    const auto begin = std::begin(shakings);
    const auto end = std::end(shakings);
    const std::size_t neighborhood_count =
        static_cast<std::size_t>(std::distance(begin, end));
    if(neighborhood_count == 0) return result;

    std::size_t neighborhood_index = 0;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto iterator = begin;
        std::advance(
            iterator,
            static_cast<typename std::iterator_traits<decltype(iterator)>::difference_type>(
                neighborhood_index
            )
        );
        State candidate = std::invoke(*iterator, current, random);
        candidate = std::invoke(local_improve, std::move(candidate));
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        result.iterations = iteration + 1;
        if(std::invoke(compare, candidate_score, current_score)){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            ++result.accepted_moves;
            neighborhood_index = 0;
            if(std::invoke(compare, current_score, result.score)){
                result.state = current;
                result.score = current_score;
            }
        }else{
            neighborhood_index = (neighborhood_index + 1) % neighborhood_count;
        }
    }
    return result;
}

template<class Constructor, class LocalImprove, class Evaluate, class URBG,
         class Compare = std::less<>>
auto grasp(
    Constructor construct,
    LocalImprove local_improve,
    Evaluate evaluate,
    std::size_t iterations,
    URBG& random,
    Compare compare = {}
){
    if(iterations == 0)[[unlikely]]{
        throw std::invalid_argument("grasp requires a positive iteration count");
    }
    using State = std::decay_t<std::invoke_result_t<Constructor&, URBG&>>;
    State initial = std::invoke(local_improve, std::invoke(construct, random));
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score initial_score = std::invoke(evaluate, initial);
    SearchResult<State, Score> result{
        std::move(initial), std::move(initial_score), iterations, 1, 0
    };
    for(std::size_t iteration = 1; iteration < iterations; ++iteration){
        State candidate = std::invoke(local_improve, std::invoke(construct, random));
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(!std::invoke(compare, candidate_score, result.score)) continue;
        result.state = std::move(candidate);
        result.score = std::move(candidate_score);
        ++result.accepted_moves;
    }
    return result;
}

template<class State, class Destroy, class Repair, class Evaluate, class URBG,
         class Compare = std::less<>>
auto destroy_and_repair(
    State initial_state,
    Destroy destroy,
    Repair repair,
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
    State current = result.state;
    Score current_score = result.score;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto partial = std::invoke(destroy, current, random);
        State candidate = std::invoke(repair, std::move(partial), random);
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

}  // namespace approximate::metaheuristic

#endif  // CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ITERATED_SEARCH_HPP_INCLUDED
