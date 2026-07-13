#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/box_constraints.hpp"
#include "detail/objective.hpp"
#include "detail/safe_count.hpp"

namespace approximate::evolutionary {

template<class Real, class Score>
struct EvolutionStrategyResult{
    std::vector<Real> individual;
    Score score;
    Real step_size;
    std::size_t iterations = 0;
    std::size_t evaluations = 0;
};

namespace evolution_strategy_detail {

template<class Real, class URBG>
std::vector<Real> gaussian_mutation(
    const std::vector<Real>& parent,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Real step_size,
    URBG& random
){
    std::normal_distribution<Real> normal(Real{}, step_size);
    std::vector<Real> child = parent;
    for(auto& coordinate: child) coordinate += normal(random);
    detail::clamp_to_box(child, lower, upper);
    return child;
}

template<bool Plus, class Real, class Evaluate, class URBG, class Compare>
auto mu_lambda_es_impl(
    std::vector<std::vector<Real>> population,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t generations,
    std::size_t lambda,
    Real step_size,
    URBG& random,
    Compare compare
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(population, lower, upper, 1);
    if(lambda == 0)[[unlikely]]{
        throw std::invalid_argument("lambda must be positive");
    }
    if constexpr(!Plus){
        if(lambda < population.size())[[unlikely]]{
            throw std::invalid_argument("(mu,lambda)-ES requires lambda >= mu");
        }
    }
    const std::size_t candidate_count = generations == 0
        ? 0
        : detail::checked_add(
            lambda, Plus ? population.size() : 0,
            "evolution strategy candidate count overflows size_t"
        );
    const std::size_t expected_evaluations = detail::checked_multiply_add(
        generations, lambda, population.size(),
        "evolution strategy evaluation count overflows size_t"
    );
    if(!(Real{} < step_size) || !std::isfinite(step_size))[[unlikely]]{
        throw std::invalid_argument("step_size must be finite and positive");
    }

    using Individual = std::vector<Real>;
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const Individual&>>;
    const std::size_t mu = population.size();
    std::vector<Score> scores;
    scores.reserve(mu);
    for(const auto& individual: population){
        scores.push_back(detail::evaluate_objective(evaluate, individual));
    }
    std::uniform_int_distribution<std::size_t> choose_parent(0, mu - 1);

    for(std::size_t generation = 0; generation < generations; ++generation){
        std::vector<Individual> candidates;
        std::vector<Score> candidate_scores;
        candidates.reserve(candidate_count);
        candidate_scores.reserve(candidate_count);
        if constexpr(Plus){
            candidates = population;
            candidate_scores = scores;
        }
        for(std::size_t child_index = 0; child_index < lambda; ++child_index){
            Individual child = gaussian_mutation(
                population[choose_parent(random)], lower, upper, step_size, random
            );
            candidate_scores.push_back(detail::evaluate_objective(evaluate, child));
            candidates.push_back(std::move(child));
        }
        std::vector<std::size_t> order(candidates.size());
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::stable_sort(order.begin(), order.end(), [&](std::size_t lhs, std::size_t rhs){
            return std::invoke(compare, candidate_scores[lhs], candidate_scores[rhs]);
        });
        std::vector<Individual> next_population;
        std::vector<Score> next_scores;
        next_population.reserve(mu);
        next_scores.reserve(mu);
        for(std::size_t index = 0; index < mu; ++index){
            next_population.push_back(std::move(candidates[order[index]]));
            next_scores.push_back(std::move(candidate_scores[order[index]]));
        }
        population = std::move(next_population);
        scores = std::move(next_scores);
    }

    std::size_t best = 0;
    for(std::size_t index = 1; index < mu; ++index){
        if(std::invoke(compare, scores[index], scores[best])) best = index;
    }
    return EvolutionStrategyResult<Real, Score>{
        population[best], scores[best], step_size, generations, expected_evaluations
    };
}

}  // namespace evolution_strategy_detail

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto one_plus_one_es(
    std::vector<Real> initial,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t iterations,
    Real step_size,
    URBG& random,
    Real success_multiplier = Real{1.5},
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(
        std::vector<std::vector<Real>>{initial}, lower, upper, 1
    );
    if(!(Real{} < step_size) || !std::isfinite(step_size))[[unlikely]]{
        throw std::invalid_argument("step_size must be finite and positive");
    }
    if(success_multiplier < Real{1} || !std::isfinite(success_multiplier))[[unlikely]]{
        throw std::invalid_argument("success_multiplier must be finite and at least one");
    }
    const std::size_t expected_evaluations = detail::checked_add(
        iterations, 1, "evolution strategy evaluation count overflows size_t"
    );
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const std::vector<Real>&>
    >;
    Score score = detail::evaluate_objective(evaluate, initial);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        auto candidate = evolution_strategy_detail::gaussian_mutation(
            initial, lower, upper, step_size, random
        );
        Score candidate_score = detail::evaluate_objective(evaluate, candidate);
        if(std::invoke(compare, candidate_score, score)){
            initial = std::move(candidate);
            score = std::move(candidate_score);
            const Real grown_step_size = step_size * success_multiplier;
            if(std::isfinite(grown_step_size)) step_size = grown_step_size;
        }else{
            step_size /= std::pow(success_multiplier, Real{0.25});
        }
        step_size = std::max(step_size, std::numeric_limits<Real>::min());
    }
    return EvolutionStrategyResult<Real, Score>{
        std::move(initial), std::move(score), step_size, iterations, expected_evaluations
    };
}

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto mu_plus_lambda_es(
    std::vector<std::vector<Real>> population,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t generations,
    std::size_t lambda,
    Real step_size,
    URBG& random,
    Compare compare = {}
){
    return evolution_strategy_detail::mu_lambda_es_impl<true>(
        std::move(population), lower, upper, std::move(evaluate), generations,
        lambda, step_size, random, std::move(compare)
    );
}

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto mu_comma_lambda_es(
    std::vector<std::vector<Real>> population,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t generations,
    std::size_t lambda,
    Real step_size,
    URBG& random,
    Compare compare = {}
){
    return evolution_strategy_detail::mu_lambda_es_impl<false>(
        std::move(population), lower, upper, std::move(evaluate), generations,
        lambda, step_size, random, std::move(compare)
    );
}

}  // namespace approximate::evolutionary
