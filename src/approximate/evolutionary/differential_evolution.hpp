#pragma once

#include <cmath>
#include <cstddef>
#include <functional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/box_constraints.hpp"
#include "detail/objective.hpp"
#include "detail/safe_count.hpp"
#include "evolution_result.hpp"

namespace approximate::evolutionary {

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto differential_evolution(
    std::vector<std::vector<Real>> population,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t generations,
    Real differential_weight,
    Real crossover_probability,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(population, lower, upper, 4);
    if(differential_weight < Real{} || !std::isfinite(differential_weight))[[unlikely]]{
        throw std::invalid_argument("differential_weight must be finite and non-negative");
    }
    if(crossover_probability < Real{} || Real{1} < crossover_probability
       || !std::isfinite(crossover_probability))[[unlikely]]{
        throw std::invalid_argument("crossover_probability must be in [0, 1]");
    }

    using Individual = std::vector<Real>;
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const Individual&>>;
    const std::size_t population_size = population.size();
    const std::size_t dimension = population.front().size();
    const std::size_t expected_evaluations = detail::checked_multiply_add(
        generations, population_size, population_size,
        "differential evolution evaluation count overflows size_t"
    );
    std::vector<Score> scores;
    scores.reserve(population_size);
    for(const auto& individual: population){
        scores.push_back(detail::evaluate_objective(evaluate, individual));
    }
    std::uniform_int_distribution<std::size_t> choose_index(0, population_size - 1);
    std::uniform_int_distribution<std::size_t> choose_coordinate(0, dimension - 1);
    std::uniform_real_distribution<Real> unit(Real{}, Real{1});

    for(std::size_t generation = 0; generation < generations; ++generation){
        auto next_population = population;
        auto next_scores = scores;
        for(std::size_t target = 0; target < population_size; ++target){
            std::size_t first;
            std::size_t second;
            std::size_t third;
            do first = choose_index(random); while(first == target);
            do second = choose_index(random);
            while(second == target || second == first);
            do third = choose_index(random);
            while(third == target || third == first || third == second);

            Individual trial = population[target];
            const std::size_t forced = choose_coordinate(random);
            for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
                if(coordinate != forced && unit(random) >= crossover_probability) continue;
                if(differential_weight == Real{}){
                    trial[coordinate] = population[first][coordinate];
                }else{
                    trial[coordinate] = population[first][coordinate]
                        + differential_weight
                            * (population[second][coordinate]
                               - population[third][coordinate]);
                }
            }
            detail::clamp_to_box(trial, lower, upper);
            Score trial_score = detail::evaluate_objective(evaluate, trial);
            if(std::invoke(compare, trial_score, scores[target])){
                next_population[target] = std::move(trial);
                next_scores[target] = std::move(trial_score);
            }
        }
        population = std::move(next_population);
        scores = std::move(next_scores);
    }

    std::size_t best = 0;
    for(std::size_t index = 1; index < population_size; ++index){
        if(std::invoke(compare, scores[index], scores[best])) best = index;
    }
    return EvolutionResult<Individual, Score>{
        population[best], scores[best], generations, expected_evaluations
    };
}

}  // namespace approximate::evolutionary
