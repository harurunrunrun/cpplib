#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "search_result.hpp"

namespace approximate::metaheuristic {

namespace internal {

template<class Score>
long double score_increase(const Score& candidate, const Score& current){
    static_assert(std::is_convertible_v<Score, long double>);
    return static_cast<long double>(candidate)
        - static_cast<long double>(current);
}

template<class State, class RandomNeighbor, class Evaluate,
         class Temperature, class URBG>
auto annealing_with_schedule(
    State initial_state,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    Temperature temperature,
    URBG& random
){
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    std::uniform_real_distribution<long double> probability(0.0L, 1.0L);

    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        const long double current_temperature =
            static_cast<long double>(std::invoke(temperature, iteration));
        if(!(current_temperature > 0.0L)
            || !std::isfinite(current_temperature))[[unlikely]]{
            throw std::invalid_argument(
                "annealing temperature must be finite and positive"
            );
        }
        State candidate = std::invoke(random_neighbor, current, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        const long double increase = score_increase(candidate_score, current_score);
        const bool accept = increase <= 0.0L
            || probability(random) < std::exp(-increase / current_temperature);
        if(!accept) continue;
        current = std::move(candidate);
        current_score = std::move(candidate_score);
        ++result.accepted_moves;
        if(current_score < result.score){
            result.state = current;
            result.score = current_score;
        }
    }
    return result;
}

}  // namespace internal

template<class State, class RandomNeighbor, class Evaluate, class URBG>
auto simulated_annealing(
    State initial_state,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    long double initial_temperature,
    long double cooling_rate,
    URBG& random
){
    if(!(initial_temperature > 0.0L)
        || !(cooling_rate > 0.0L && cooling_rate <= 1.0L))[[unlikely]]{
        throw std::invalid_argument("invalid simulated annealing schedule");
    }
    const auto schedule = [initial_temperature, cooling_rate](std::size_t step){
        const long double value = initial_temperature * std::pow(
            cooling_rate, static_cast<long double>(step)
        );
        return std::max(value, std::numeric_limits<long double>::min());
    };
    return internal::annealing_with_schedule(
        std::move(initial_state), std::move(random_neighbor),
        std::move(evaluate), iterations, schedule, random
    );
}

template<class State, class RandomNeighbor, class Evaluate, class URBG>
auto fast_simulated_annealing(
    State initial_state,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    long double initial_temperature,
    URBG& random
){
    if(!(initial_temperature > 0.0L))[[unlikely]]{
        throw std::invalid_argument(
            "fast_simulated_annealing requires a positive temperature"
        );
    }
    const auto schedule = [initial_temperature](std::size_t step){
        return initial_temperature
            / static_cast<long double>(step + std::size_t{1});
    };
    return internal::annealing_with_schedule(
        std::move(initial_state), std::move(random_neighbor),
        std::move(evaluate), iterations, schedule, random
    );
}

template<class State, class RandomNeighbor, class Evaluate, class URBG>
auto threshold_accepting(
    State initial_state,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    long double initial_threshold,
    long double cooling_rate,
    URBG& random
){
    if(initial_threshold < 0.0L
        || !(cooling_rate >= 0.0L && cooling_rate <= 1.0L))[[unlikely]]{
        throw std::invalid_argument("invalid threshold accepting schedule");
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    long double threshold = initial_threshold;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        State candidate = std::invoke(random_neighbor, current, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(internal::score_increase(candidate_score, current_score) <= threshold){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            ++result.accepted_moves;
            if(current_score < result.score){
                result.state = current;
                result.score = current_score;
            }
        }
        threshold *= cooling_rate;
    }
    return result;
}

template<class State, class RandomNeighbor, class Evaluate, class URBG>
auto great_deluge(
    State initial_state,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    long double initial_water_level,
    long double decay,
    URBG& random
){
    if(decay < 0.0L)[[unlikely]]{
        throw std::invalid_argument("great_deluge requires nonnegative decay");
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    long double water_level = initial_water_level;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        State candidate = std::invoke(random_neighbor, current, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        const bool improve = candidate_score < current_score;
        const bool under_level =
            static_cast<long double>(candidate_score) <= water_level;
        if(improve || under_level){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            ++result.accepted_moves;
            if(current_score < result.score){
                result.state = current;
                result.score = current_score;
            }
        }
        water_level -= decay;
    }
    return result;
}

template<class State, class RandomNeighbor, class Evaluate, class URBG>
auto late_acceptance_hill_climbing(
    State initial_state,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    std::size_t history_length,
    URBG& random
){
    if(history_length == 0)[[unlikely]]{
        throw std::invalid_argument(
            "late_acceptance_hill_climbing requires nonempty history"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    std::vector<Score> history(history_length, current_score);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        const std::size_t slot = iteration % history_length;
        State candidate = std::invoke(random_neighbor, current, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(candidate_score <= current_score || candidate_score <= history[slot]){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            ++result.accepted_moves;
            if(current_score < result.score){
                result.state = current;
                result.score = current_score;
            }
        }
        history[slot] = current_score;
    }
    return result;
}

}  // namespace approximate::metaheuristic
