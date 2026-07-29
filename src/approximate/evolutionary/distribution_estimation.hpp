#ifndef CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_DISTRIBUTION_ESTIMATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_DISTRIBUTION_ESTIMATION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "evolution_result.hpp"
#include "detail/objective.hpp"
#include "detail/safe_count.hpp"

namespace approximate::evolutionary {

template<class Real, class Score>
struct DistributionEstimationResult{
    std::vector<std::uint8_t> individual;
    Score score;
    std::vector<Real> probabilities;
    std::size_t iterations = 0;
    std::size_t evaluations = 0;
};

template<class Parameters, class Sample, class Update, class Evaluate,
         class URBG, class Compare = std::less<>>
auto estimation_of_distribution_algorithm(
    Parameters parameters,
    Sample sample,
    Update update,
    Evaluate evaluate,
    std::size_t generations,
    std::size_t population_size,
    std::size_t elite_count,
    URBG& random,
    Compare compare = {}
){
    if(generations == 0 || population_size == 0 || elite_count == 0
       || population_size < elite_count)[[unlikely]]{
        throw std::invalid_argument(
            "estimation_of_distribution_algorithm has invalid sizes"
        );
    }
    using Individual = std::decay_t<
        std::invoke_result_t<Sample&, const Parameters&, URBG&>
    >;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    const std::size_t evaluations = detail::checked_multiply(
        generations, population_size,
        "EDA evaluation count overflows size_t"
    );
    std::optional<Individual> best;
    std::optional<Score> best_score;
    for(std::size_t generation = 0; generation < generations; ++generation){
        std::vector<Individual> population;
        std::vector<Score> scores;
        population.reserve(population_size);
        scores.reserve(population_size);
        for(std::size_t i = 0; i < population_size; ++i){
            Individual individual = std::invoke(sample, parameters, random);
            Score score = detail::evaluate_objective(evaluate, individual);
            if(!best_score || std::invoke(compare, score, *best_score)){
                best = individual;
                best_score = score;
            }
            population.push_back(std::move(individual));
            scores.push_back(std::move(score));
        }
        std::vector<std::size_t> order(population_size);
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::stable_sort(
            order.begin(), order.end(),
            [&](std::size_t lhs, std::size_t rhs){
                return std::invoke(compare, scores[lhs], scores[rhs]);
            }
        );
        std::vector<Individual> elites;
        elites.reserve(elite_count);
        for(std::size_t i = 0; i < elite_count; ++i){
            elites.push_back(std::move(population[order[i]]));
        }
        parameters = std::invoke(
            update, std::as_const(elites), std::move(parameters)
        );
    }
    return EvolutionResult<Individual, Score>{
        std::move(*best), std::move(*best_score), generations, evaluations
    };
}

template<class Real = double, class Evaluate, class URBG,
         class Compare = std::less<>>
auto univariate_marginal_distribution_algorithm(
    std::size_t bit_count,
    Evaluate evaluate,
    std::size_t generations,
    std::size_t population_size,
    std::size_t elite_count,
    URBG& random,
    Real margin = Real{},
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    if(bit_count == 0 || generations == 0 || population_size == 0
       || elite_count == 0 || population_size < elite_count
       || margin < Real{} || !(margin < Real{0.5})
       || !std::isfinite(margin))[[unlikely]]{
        throw std::invalid_argument("UMDA has invalid sizes or margin");
    }
    using Individual = std::vector<std::uint8_t>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    const std::size_t evaluations = detail::checked_multiply(
        generations, population_size,
        "UMDA evaluation count overflows size_t"
    );
    std::vector<Real> probabilities(bit_count, Real{0.5});
    std::optional<Individual> best;
    std::optional<Score> best_score;
    for(std::size_t generation = 0; generation < generations; ++generation){
        std::vector<Individual> population(
            population_size, Individual(bit_count)
        );
        std::vector<Score> scores;
        scores.reserve(population_size);
        for(Individual& individual : population){
            for(std::size_t bit = 0; bit < bit_count; ++bit){
                std::bernoulli_distribution choose(
                    static_cast<double>(probabilities[bit])
                );
                individual[bit] =
                    static_cast<std::uint8_t>(choose(random));
            }
            Score score = detail::evaluate_objective(evaluate, individual);
            if(!best_score || std::invoke(compare, score, *best_score)){
                best = individual;
                best_score = score;
            }
            scores.push_back(std::move(score));
        }
        std::vector<std::size_t> order(population_size);
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::stable_sort(
            order.begin(), order.end(),
            [&](std::size_t lhs, std::size_t rhs){
                return std::invoke(compare, scores[lhs], scores[rhs]);
            }
        );
        std::fill(probabilities.begin(), probabilities.end(), Real{});
        for(std::size_t rank = 0; rank < elite_count; ++rank){
            const Individual& elite = population[order[rank]];
            for(std::size_t bit = 0; bit < bit_count; ++bit){
                probabilities[bit] += static_cast<Real>(elite[bit])
                    / static_cast<Real>(elite_count);
            }
        }
        for(Real& probability : probabilities){
            probability = std::clamp(
                probability, margin, Real{1} - margin
            );
        }
    }
    return DistributionEstimationResult<Real, Score>{
        std::move(*best), std::move(*best_score), std::move(probabilities),
        generations, evaluations
    };
}

template<class Real = double, class Evaluate, class URBG,
         class Compare = std::less<>>
auto population_based_incremental_learning(
    std::size_t bit_count,
    Evaluate evaluate,
    std::size_t generations,
    std::size_t population_size,
    Real learning_rate,
    Real mutation_probability,
    Real mutation_shift,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    if(bit_count == 0 || generations == 0 || population_size == 0
       || !(learning_rate > Real{} && learning_rate <= Real{1})
       || mutation_probability < Real{}
       || Real{1} < mutation_probability
       || mutation_shift < Real{} || Real{1} < mutation_shift
       || !std::isfinite(learning_rate)
       || !std::isfinite(mutation_probability)
       || !std::isfinite(mutation_shift))[[unlikely]]{
        throw std::invalid_argument("PBIL has invalid parameters");
    }
    using Individual = std::vector<std::uint8_t>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    const std::size_t evaluations = detail::checked_multiply(
        generations, population_size,
        "PBIL evaluation count overflows size_t"
    );
    std::vector<Real> probabilities(bit_count, Real{0.5});
    std::optional<Individual> best;
    std::optional<Score> best_score;
    std::bernoulli_distribution random_bit(0.5);
    for(std::size_t generation = 0; generation < generations; ++generation){
        std::optional<Individual> generation_best;
        std::optional<Score> generation_best_score;
        for(std::size_t i = 0; i < population_size; ++i){
            Individual individual(bit_count);
            for(std::size_t bit = 0; bit < bit_count; ++bit){
                std::bernoulli_distribution choose(
                    static_cast<double>(probabilities[bit])
                );
                individual[bit] =
                    static_cast<std::uint8_t>(choose(random));
            }
            Score score = detail::evaluate_objective(evaluate, individual);
            if(!generation_best_score
               || std::invoke(
                   compare, score, *generation_best_score
               )){
                generation_best = individual;
                generation_best_score = score;
            }
            if(!best_score || std::invoke(compare, score, *best_score)){
                best = std::move(individual);
                best_score = std::move(score);
            }
        }
        for(std::size_t bit = 0; bit < bit_count; ++bit){
            probabilities[bit] =
                (Real{1} - learning_rate) * probabilities[bit]
                + learning_rate
                    * static_cast<Real>((*generation_best)[bit]);
            std::bernoulli_distribution mutate(
                static_cast<double>(mutation_probability)
            );
            if(mutate(random)){
                probabilities[bit] =
                    (Real{1} - mutation_shift) * probabilities[bit]
                    + mutation_shift
                        * static_cast<Real>(random_bit(random));
            }
            probabilities[bit] = std::clamp(
                probabilities[bit], Real{}, Real{1}
            );
        }
    }
    return DistributionEstimationResult<Real, Score>{
        std::move(*best), std::move(*best_score), std::move(probabilities),
        generations, evaluations
    };
}

}  // namespace approximate::evolutionary

#endif  // CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_DISTRIBUTION_ESTIMATION_HPP_INCLUDED
