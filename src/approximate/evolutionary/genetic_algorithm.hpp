#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/objective.hpp"
#include "detail/safe_count.hpp"
#include "evolution_result.hpp"

namespace approximate::evolutionary {

namespace genetic_algorithm_detail {

template<class Score, class Compare>
std::vector<std::size_t> sorted_indices(
    const std::vector<Score>& scores,
    Compare& compare
){
    std::vector<std::size_t> order(scores.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::stable_sort(order.begin(), order.end(), [&](std::size_t lhs, std::size_t rhs){
        return std::invoke(compare, scores[lhs], scores[rhs]);
    });
    return order;
}

template<class Individual, class Score, class Compare>
EvolutionResult<Individual, Score> make_result(
    const std::vector<Individual>& population,
    const std::vector<Score>& scores,
    std::size_t iterations,
    std::size_t evaluations,
    Compare& compare
){
    std::size_t best = 0;
    for(std::size_t index = 1; index < population.size(); ++index){
        if(std::invoke(compare, scores[index], scores[best])) best = index;
    }
    return EvolutionResult<Individual, Score>{
        population[best], scores[best], iterations, evaluations
    };
}

template<class Select, class Population, class Scores, class URBG>
std::size_t select_index(
    Select& select,
    const Population& population,
    const Scores& scores,
    URBG& random
){
    using Selection = std::decay_t<std::invoke_result_t<
        Select&, const Population&, const Scores&, URBG&
    >>;
    static_assert(
        std::is_integral_v<Selection>,
        "genetic algorithm selectors must return an integral index"
    );
    const Selection selected = std::invoke(select, population, scores, random);
    if constexpr(std::is_signed_v<Selection>){
        if(selected < 0)[[unlikely]]{
            throw std::out_of_range("genetic algorithm selector returned an invalid index");
        }
    }
    if constexpr(sizeof(Selection) > sizeof(std::size_t)){
        using UnsignedSelection = std::make_unsigned_t<Selection>;
        if(static_cast<UnsignedSelection>(selected)
           > static_cast<UnsignedSelection>(std::numeric_limits<std::size_t>::max())){
            throw std::out_of_range("genetic algorithm selector returned an invalid index");
        }
    }
    const std::size_t index = static_cast<std::size_t>(selected);
    if(index >= population.size())[[unlikely]]{
        throw std::out_of_range("genetic algorithm selector returned an invalid index");
    }
    return index;
}

}  // namespace genetic_algorithm_detail

template<class Individual, class Evaluate, class Select, class Crossover,
         class Mutate, class URBG, class Compare = std::less<>>
auto generational_genetic_algorithm(
    std::vector<Individual> population,
    Evaluate evaluate,
    Select select,
    Crossover crossover,
    Mutate mutate,
    std::size_t generations,
    URBG& random,
    std::size_t elite_count = 1,
    Compare compare = {}
){
    if(population.empty())[[unlikely]]{
        throw std::invalid_argument("genetic algorithm requires a non-empty population");
    }
    if(elite_count > population.size())[[unlikely]]{
        throw std::invalid_argument("elite_count exceeds the population size");
    }
    const std::size_t expected_evaluations = detail::checked_multiply_add(
        generations, population.size(), population.size(),
        "genetic algorithm evaluation count overflows size_t"
    );
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    std::vector<Score> scores;
    scores.reserve(population.size());
    for(const auto& individual: population){
        scores.push_back(detail::evaluate_objective(evaluate, individual));
    }

    for(std::size_t generation = 0; generation < generations; ++generation){
        const auto order = genetic_algorithm_detail::sorted_indices(scores, compare);
        std::vector<Individual> next;
        next.reserve(population.size());
        for(std::size_t index = 0; index < elite_count; ++index){
            next.push_back(population[order[index]]);
        }
        while(next.size() < population.size()){
            const std::size_t lhs = genetic_algorithm_detail::select_index(
                select, population, scores, random
            );
            const std::size_t rhs = genetic_algorithm_detail::select_index(
                select, population, scores, random
            );
            Individual child = std::invoke(
                crossover, std::as_const(population[lhs]),
                std::as_const(population[rhs]), random
            );
            std::invoke(mutate, child, random);
            next.push_back(std::move(child));
        }
        population = std::move(next);
        scores.clear();
        for(const auto& individual: population){
            scores.push_back(detail::evaluate_objective(evaluate, individual));
        }
    }
    return genetic_algorithm_detail::make_result(
        population, scores, generations, expected_evaluations, compare
    );
}

template<class Individual, class Evaluate, class Select, class Crossover,
         class Mutate, class URBG, class Compare = std::less<>>
auto steady_state_genetic_algorithm(
    std::vector<Individual> population,
    Evaluate evaluate,
    Select select,
    Crossover crossover,
    Mutate mutate,
    std::size_t offspring_count,
    URBG& random,
    Compare compare = {}
){
    if(population.empty())[[unlikely]]{
        throw std::invalid_argument("genetic algorithm requires a non-empty population");
    }
    const std::size_t expected_evaluations = detail::checked_add(
        population.size(), offspring_count,
        "genetic algorithm evaluation count overflows size_t"
    );
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const Individual&>>;
    std::vector<Score> scores;
    scores.reserve(population.size());
    for(const auto& individual: population){
        scores.push_back(detail::evaluate_objective(evaluate, individual));
    }

    for(std::size_t iteration = 0; iteration < offspring_count; ++iteration){
        const std::size_t lhs = genetic_algorithm_detail::select_index(
            select, population, scores, random
        );
        const std::size_t rhs = genetic_algorithm_detail::select_index(
            select, population, scores, random
        );
        Individual child = std::invoke(
            crossover, std::as_const(population[lhs]),
            std::as_const(population[rhs]), random
        );
        std::invoke(mutate, child, random);
        Score child_score = detail::evaluate_objective(evaluate, child);

        std::size_t worst = 0;
        for(std::size_t index = 1; index < population.size(); ++index){
            if(std::invoke(compare, scores[worst], scores[index])) worst = index;
        }
        if(std::invoke(compare, child_score, scores[worst])){
            population[worst] = std::move(child);
            scores[worst] = std::move(child_score);
        }
    }
    return genetic_algorithm_detail::make_result(
        population, scores, offspring_count, expected_evaluations, compare
    );
}

}  // namespace approximate::evolutionary
