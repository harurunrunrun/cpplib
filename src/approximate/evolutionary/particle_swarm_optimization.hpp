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
auto particle_swarm_optimization(
    std::vector<std::vector<Real>> positions,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t iterations,
    Real inertia,
    Real cognitive_coefficient,
    Real social_coefficient,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(positions, lower, upper, 1);
    if(inertia < Real{} || cognitive_coefficient < Real{}
       || social_coefficient < Real{} || !std::isfinite(inertia)
       || !std::isfinite(cognitive_coefficient)
       || !std::isfinite(social_coefficient))[[unlikely]]{
        throw std::invalid_argument("PSO coefficients must be finite and non-negative");
    }

    using Individual = std::vector<Real>;
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const Individual&>>;
    const std::size_t population_size = positions.size();
    const std::size_t dimension = positions.front().size();
    const std::size_t expected_evaluations = detail::checked_multiply_add(
        iterations, population_size, population_size,
        "particle swarm evaluation count overflows size_t"
    );
    std::vector<Individual> velocities(
        population_size, Individual(dimension, Real{})
    );
    std::vector<Individual> personal_best = positions;
    std::vector<Score> personal_scores;
    personal_scores.reserve(population_size);
    for(const auto& position: positions){
        personal_scores.push_back(detail::evaluate_objective(evaluate, position));
    }
    std::size_t global = 0;
    for(std::size_t index = 1; index < population_size; ++index){
        if(std::invoke(compare, personal_scores[index], personal_scores[global])){
            global = index;
        }
    }
    Individual global_best = personal_best[global];
    Score global_score = personal_scores[global];
    std::uniform_real_distribution<Real> unit(Real{}, Real{1});

    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        for(std::size_t particle = 0; particle < population_size; ++particle){
            for(std::size_t coordinate = 0; coordinate < dimension; ++coordinate){
                Real velocity = Real{};
                if(inertia != Real{}){
                    velocity += inertia * velocities[particle][coordinate];
                }
                if(cognitive_coefficient != Real{}){
                    velocity += cognitive_coefficient * unit(random)
                        * (personal_best[particle][coordinate]
                           - positions[particle][coordinate]);
                }
                if(social_coefficient != Real{}){
                    velocity += social_coefficient * unit(random)
                        * (global_best[coordinate] - positions[particle][coordinate]);
                }
                if(std::isnan(velocity)) velocity = Real{};
                velocities[particle][coordinate] = velocity;
                positions[particle][coordinate] += velocities[particle][coordinate];
                if(std::isnan(positions[particle][coordinate])){
                    positions[particle][coordinate] = personal_best[particle][coordinate];
                    velocities[particle][coordinate] = Real{};
                }else if(positions[particle][coordinate] < lower[coordinate]){
                    positions[particle][coordinate] = lower[coordinate];
                    velocities[particle][coordinate] = Real{};
                }else if(upper[coordinate] < positions[particle][coordinate]){
                    positions[particle][coordinate] = upper[coordinate];
                    velocities[particle][coordinate] = Real{};
                }
            }
            Score score = detail::evaluate_objective(evaluate, positions[particle]);
            if(std::invoke(compare, score, personal_scores[particle])){
                personal_best[particle] = positions[particle];
                personal_scores[particle] = score;
                if(std::invoke(compare, score, global_score)){
                    global_best = personal_best[particle];
                    global_score = std::move(score);
                }
            }
        }
    }
    return EvolutionResult<Individual, Score>{
        std::move(global_best), std::move(global_score), iterations, expected_evaluations
    };
}

}  // namespace approximate::evolutionary
