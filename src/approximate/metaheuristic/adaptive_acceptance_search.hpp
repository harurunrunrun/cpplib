#ifndef CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ADAPTIVE_ACCEPTANCE_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ADAPTIVE_ACCEPTANCE_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "search_result.hpp"

namespace approximate::metaheuristic {

template<class State, class RandomNeighbor, class Evaluate, class URBG>
auto adaptive_simulated_annealing(
    State initial_state,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    long double initial_temperature,
    std::size_t adaptation_interval,
    long double target_acceptance_rate,
    long double adaptation_factor,
    URBG& random
){
    if(!(initial_temperature > 0.0L)
       || !std::isfinite(initial_temperature)
       || adaptation_interval == 0
       || target_acceptance_rate < 0.0L
       || target_acceptance_rate > 1.0L
       || !(adaptation_factor > 1.0L)
       || !std::isfinite(adaptation_factor))[[unlikely]]{
        throw std::invalid_argument(
            "adaptive_simulated_annealing has an invalid schedule"
        );
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    long double temperature = initial_temperature;
    std::size_t window_accepted = 0;
    std::uniform_real_distribution<long double> probability(0.0L, 1.0L);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        State candidate = std::invoke(random_neighbor, current, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        const long double increase =
            static_cast<long double>(candidate_score)
            - static_cast<long double>(current_score);
        const bool accept = increase <= 0.0L
            || probability(random) < std::exp(-increase / temperature);
        if(accept){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            ++result.accepted_moves;
            ++window_accepted;
            if(current_score < result.score){
                result.state = current;
                result.score = current_score;
            }
        }
        if((iteration + 1) % adaptation_interval != 0) continue;
        const long double rate = static_cast<long double>(window_accepted)
            / static_cast<long double>(adaptation_interval);
        if(rate < target_acceptance_rate){
            temperature *= adaptation_factor;
        }else{
            temperature /= adaptation_factor;
        }
        temperature = std::clamp(
            temperature,
            std::numeric_limits<long double>::min(),
            std::numeric_limits<long double>::max()
        );
        window_accepted = 0;
    }
    return result;
}

template<class Generator, class Evaluate, class Step, class URBG,
         class Compare = std::less<>>
auto adaptive_random_search(
    Generator generator,
    Evaluate evaluate,
    std::size_t samples,
    Step initial_step,
    Step expansion_factor,
    Step contraction_factor,
    std::size_t failure_limit,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_arithmetic_v<Step>);
    if(samples == 0
       || !(Step{} < initial_step)
       || !(Step{1} < expansion_factor)
       || !(Step{} < contraction_factor
            && contraction_factor < Step{1})
       || failure_limit == 0)[[unlikely]]{
        throw std::invalid_argument(
            "adaptive_random_search has invalid adaptation parameters"
        );
    }
    using State = std::decay_t<
        std::invoke_result_t<Generator&, Step, URBG&>
    >;
    State best = std::invoke(generator, initial_step, random);
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    Score best_score = std::invoke(evaluate, best);
    SearchResult<State, Score> result{
        best, best_score, samples, 1, 0
    };
    Step step = initial_step;
    std::size_t failures = 0;
    for(std::size_t sample = 1; sample < samples; ++sample){
        State candidate = std::invoke(generator, step, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        if(std::invoke(compare, candidate_score, best_score)){
            best = std::move(candidate);
            best_score = std::move(candidate_score);
            result.state = best;
            result.score = best_score;
            ++result.accepted_moves;
            failures = 0;
            const Step expanded = static_cast<Step>(
                step * expansion_factor
            );
            if constexpr(std::is_floating_point_v<Step>){
                if(std::isfinite(expanded)) step = expanded;
            }else{
                if(step <= std::numeric_limits<Step>::max()
                               / expansion_factor){
                    step = expanded;
                }
            }
        }else if(++failures == failure_limit){
            step = static_cast<Step>(step * contraction_factor);
            if(!(Step{} < step)) step = std::numeric_limits<Step>::min();
            failures = 0;
        }
    }
    return result;
}

template<class State, class RandomNeighbor, class Evaluate, class URBG>
auto parallel_tempering(
    std::vector<State> replicas,
    const std::vector<long double>& temperatures,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    std::size_t exchange_interval,
    URBG& random
){
    if(replicas.empty() || replicas.size() != temperatures.size()
       || exchange_interval == 0)[[unlikely]]{
        throw std::invalid_argument(
            "parallel_tempering requires matching non-empty replicas and "
            "temperatures and a positive exchange interval"
        );
    }
    for(const long double temperature : temperatures){
        if(!(temperature > 0.0L)
           || !std::isfinite(temperature))[[unlikely]]{
            throw std::invalid_argument(
                "parallel_tempering requires finite positive temperatures"
            );
        }
    }
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const State&>
    >;
    std::vector<Score> scores;
    scores.reserve(replicas.size());
    for(const State& state : replicas){
        scores.push_back(std::invoke(evaluate, state));
    }
    std::size_t best = 0;
    for(std::size_t i = 1; i < scores.size(); ++i){
        if(scores[i] < scores[best]) best = i;
    }
    SearchResult<State, Score> result{
        replicas[best], scores[best], iterations, replicas.size(), 0
    };
    std::uniform_real_distribution<long double> probability(0.0L, 1.0L);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        for(std::size_t i = 0; i < replicas.size(); ++i){
            State candidate = std::invoke(
                random_neighbor, replicas[i], random
            );
            Score candidate_score = std::invoke(evaluate, candidate);
            ++result.evaluations;
            const long double increase =
                static_cast<long double>(candidate_score)
                - static_cast<long double>(scores[i]);
            if(increase <= 0.0L
               || probability(random)
                    < std::exp(-increase / temperatures[i])){
                replicas[i] = std::move(candidate);
                scores[i] = std::move(candidate_score);
                ++result.accepted_moves;
                if(scores[i] < result.score){
                    result.state = replicas[i];
                    result.score = scores[i];
                }
            }
        }
        if((iteration + 1) % exchange_interval != 0) continue;
        const std::size_t parity =
            ((iteration + 1) / exchange_interval) & 1U;
        for(std::size_t i = parity; i + 1 < replicas.size(); i += 2){
            const long double exponent =
                (1.0L / temperatures[i]
                 - 1.0L / temperatures[i + 1])
                * (static_cast<long double>(scores[i])
                   - static_cast<long double>(scores[i + 1]));
            if(exponent >= 0.0L
               || probability(random) < std::exp(exponent)){
                std::swap(replicas[i], replicas[i + 1]);
                std::swap(scores[i], scores[i + 1]);
                ++result.accepted_moves;
            }
        }
    }
    return result;
}

template<class State, class RandomNeighbor, class Evaluate, class URBG>
auto simulated_tempering(
    State initial_state,
    const std::vector<long double>& temperatures,
    RandomNeighbor random_neighbor,
    Evaluate evaluate,
    std::size_t iterations,
    long double temperature_move_probability,
    URBG& random
){
    if(temperatures.empty()
       || temperature_move_probability < 0.0L
       || temperature_move_probability > 1.0L)[[unlikely]]{
        throw std::invalid_argument(
            "simulated_tempering has invalid temperatures or move probability"
        );
    }
    for(const long double temperature : temperatures){
        if(!(temperature > 0.0L)
           || !std::isfinite(temperature))[[unlikely]]{
            throw std::invalid_argument(
                "simulated_tempering requires finite positive temperatures"
            );
        }
    }
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, State&>>;
    State current = std::move(initial_state);
    Score current_score = std::invoke(evaluate, current);
    SearchResult<State, Score> result{
        current, current_score, iterations, 1, 0
    };
    std::size_t level = 0;
    std::uniform_real_distribution<long double> probability(0.0L, 1.0L);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        if(temperatures.size() > 1
           && probability(random) < temperature_move_probability){
            std::size_t candidate_level;
            if(level == 0){
                candidate_level = 1;
            }else if(level + 1 == temperatures.size()){
                candidate_level = level - 1;
            }else{
                std::bernoulli_distribution direction(0.5);
                candidate_level =
                    direction(random) ? level + 1 : level - 1;
            }
            const long double exponent =
                (1.0L / temperatures[level]
                 - 1.0L / temperatures[candidate_level])
                * static_cast<long double>(current_score);
            if(exponent >= 0.0L
               || probability(random) < std::exp(exponent)){
                level = candidate_level;
                ++result.accepted_moves;
            }
            continue;
        }
        State candidate = std::invoke(random_neighbor, current, random);
        Score candidate_score = std::invoke(evaluate, candidate);
        ++result.evaluations;
        const long double increase =
            static_cast<long double>(candidate_score)
            - static_cast<long double>(current_score);
        if(increase <= 0.0L
           || probability(random)
                < std::exp(-increase / temperatures[level])){
            current = std::move(candidate);
            current_score = std::move(candidate_score);
            ++result.accepted_moves;
            if(current_score < result.score){
                result.state = current;
                result.score = current_score;
            }
        }
    }
    return result;
}

}  // namespace approximate::metaheuristic

#endif  // CPPLIB_SRC_APPROXIMATE_METAHEURISTIC_ADAPTIVE_ACCEPTANCE_SEARCH_HPP_INCLUDED
