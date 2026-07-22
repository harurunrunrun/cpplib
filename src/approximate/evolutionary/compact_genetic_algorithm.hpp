#ifndef CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_COMPACT_GENETIC_ALGORITHM_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_COMPACT_GENETIC_ALGORITHM_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "detail/objective.hpp"
#include "detail/safe_count.hpp"


namespace approximate::evolutionary {

template<class Real, class Score>
struct CompactGeneticAlgorithmResult{
    std::vector<std::uint8_t> individual;
    Score score;
    std::vector<Real> probabilities;
    std::size_t iterations = 0;
    std::size_t evaluations = 0;
};

template<class Real = double, class Evaluate, class URBG,
         class Compare = std::less<>>
auto compact_genetic_algorithm(
    std::size_t bit_count,
    Evaluate evaluate,
    std::size_t iterations,
    std::size_t virtual_population_size,
    URBG& random,
    Compare compare = {},
    Real initial_probability = Real{0.5}
){
    static_assert(std::is_floating_point_v<Real>);
    if(bit_count == 0)[[unlikely]]{
        throw std::invalid_argument("compact GA requires at least one bit");
    }
    if(iterations == 0)[[unlikely]]{
        throw std::invalid_argument("compact GA requires at least one iteration");
    }
    if(virtual_population_size < 2)[[unlikely]]{
        throw std::invalid_argument("virtual_population_size must be at least two");
    }
    if(initial_probability < Real{} || Real{1} < initial_probability
       || !std::isfinite(initial_probability))[[unlikely]]{
        throw std::invalid_argument("initial_probability must be in [0, 1]");
    }

    using Individual = std::vector<std::uint8_t>;
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const Individual&>>;
    const std::size_t expected_evaluations = detail::checked_multiply(
        iterations, 2, "compact GA evaluation count overflows size_t"
    );
    const Real step = Real{1} / static_cast<Real>(virtual_population_size);
    const Real minimum_probability = step;
    const Real maximum_probability = Real{1} - step;
    std::vector<Real> probabilities(
        bit_count, std::clamp(initial_probability, minimum_probability, maximum_probability)
    );
    std::optional<Individual> best;
    std::optional<Score> best_score;

    const auto sample = [&](){
        Individual individual(bit_count);
        for(std::size_t index = 0; index < bit_count; ++index){
            std::bernoulli_distribution bit(
                static_cast<double>(probabilities[index])
            );
            individual[index] = static_cast<std::uint8_t>(bit(random));
        }
        return individual;
    };

    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        Individual first = sample();
        Individual second = sample();
        Score first_score = detail::evaluate_objective(evaluate, first);
        Score second_score = detail::evaluate_objective(evaluate, second);
        if(!best_score || std::invoke(compare, first_score, *best_score)){
            best = first;
            best_score = first_score;
        }
        if(std::invoke(compare, second_score, *best_score)){
            best = second;
            best_score = second_score;
        }
        const bool second_wins = std::invoke(compare, second_score, first_score);
        const Individual& winner = second_wins ? second : first;
        const Individual& loser = second_wins ? first : second;
        for(std::size_t index = 0; index < bit_count; ++index){
            if(winner[index] == loser[index]) continue;
            probabilities[index] += winner[index] != 0U ? step : -step;
            probabilities[index] = std::clamp(
                probabilities[index], minimum_probability, maximum_probability
            );
        }
    }
    return CompactGeneticAlgorithmResult<Real, Score>{
        std::move(*best), std::move(*best_score), std::move(probabilities),
        iterations, expected_evaluations
    };
}

}  // namespace approximate::evolutionary

#endif  // CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_COMPACT_GENETIC_ALGORITHM_HPP_INCLUDED
