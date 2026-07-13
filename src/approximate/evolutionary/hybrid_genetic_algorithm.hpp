#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "genetic_algorithm.hpp"

namespace approximate::evolutionary {

template<class Individual, class Evaluate, class Select, class Crossover,
         class Mutate, class LocalImprove, class URBG,
         class Compare = std::less<>>
auto memetic_genetic_algorithm(
    std::vector<Individual> population,
    Evaluate evaluate,
    Select select,
    Crossover crossover,
    Mutate mutate,
    LocalImprove local_improve,
    std::size_t generations,
    URBG& random,
    std::size_t elite_count = 1,
    Compare compare = {}
){
    if(population.empty())[[unlikely]]{
        throw std::invalid_argument("memetic algorithm requires a non-empty population");
    }
    if(elite_count > population.size())[[unlikely]]{
        throw std::invalid_argument("elite_count exceeds the population size");
    }
    const std::size_t expected_evaluations = detail::checked_multiply_add(
        generations, population.size(), population.size(),
        "memetic algorithm evaluation count overflows size_t"
    );
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const Individual&>>;
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
            std::invoke(local_improve, child, random);
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
auto island_model_genetic_algorithm(
    std::vector<std::vector<Individual>> islands,
    Evaluate evaluate,
    Select select,
    Crossover crossover,
    Mutate mutate,
    std::size_t epochs,
    std::size_t generations_per_epoch,
    std::size_t migrants_per_epoch,
    URBG& random,
    std::size_t elite_count = 1,
    Compare compare = {}
){
    if(islands.empty())[[unlikely]]{
        throw std::invalid_argument("island model requires at least one island");
    }
    std::size_t total_population = 0;
    for(const auto& island: islands){
        if(island.empty())[[unlikely]]{
            throw std::invalid_argument("islands must be non-empty");
        }
        if(elite_count > island.size())[[unlikely]]{
            throw std::invalid_argument("elite_count exceeds an island size");
        }
        if(migrants_per_epoch > island.size())[[unlikely]]{
            throw std::invalid_argument("migrant count exceeds an island size");
        }
        total_population = detail::checked_add(total_population, island.size(), "island population size overflows size_t");
    }
    const std::size_t local_generations = detail::checked_multiply(
        epochs, generations_per_epoch,
        "island model generation count overflows size_t"
    );
    const std::size_t expected_evaluations = detail::checked_multiply_add(
        local_generations, total_population, total_population,
        "island model evaluation count overflows size_t"
    );
    using Score = std::decay_t<std::invoke_result_t<Evaluate&, const Individual&>>;
    std::vector<std::vector<Score>> scores(islands.size());
    for(std::size_t island_index = 0; island_index < islands.size(); ++island_index){
        scores[island_index].reserve(islands[island_index].size());
        for(const auto& individual: islands[island_index]){
            scores[island_index].push_back(detail::evaluate_objective(evaluate, individual));
        }
    }

    for(std::size_t epoch = 0; epoch < epochs; ++epoch){
        for(std::size_t local_generation = 0;
            local_generation < generations_per_epoch;
            ++local_generation){
            for(std::size_t island_index = 0;
                island_index < islands.size();
                ++island_index){
                auto& population = islands[island_index];
                auto& island_scores = scores[island_index];
                const auto order = genetic_algorithm_detail::sorted_indices(
                    island_scores, compare
                );
                std::vector<Individual> next;
                next.reserve(population.size());
                for(std::size_t index = 0; index < elite_count; ++index){
                    next.push_back(population[order[index]]);
                }
                while(next.size() < population.size()){
                    const std::size_t lhs = genetic_algorithm_detail::select_index(
                        select, population, island_scores, random
                    );
                    const std::size_t rhs = genetic_algorithm_detail::select_index(
                        select, population, island_scores, random
                    );
                    Individual child = std::invoke(
                        crossover, std::as_const(population[lhs]),
                        std::as_const(population[rhs]), random
                    );
                    std::invoke(mutate, child, random);
                    next.push_back(std::move(child));
                }
                population = std::move(next);
                island_scores.clear();
                for(const auto& individual: population){
                    island_scores.push_back(detail::evaluate_objective(evaluate, individual));
                }
            }
        }

        if(migrants_per_epoch == 0) continue;
        std::vector<std::vector<Individual>> migrants(islands.size());
        std::vector<std::vector<Score>> migrant_scores(islands.size());
        for(std::size_t source = 0; source < islands.size(); ++source){
            const auto order = genetic_algorithm_detail::sorted_indices(
                scores[source], compare
            );
            migrants[source].reserve(migrants_per_epoch);
            migrant_scores[source].reserve(migrants_per_epoch);
            for(std::size_t index = 0; index < migrants_per_epoch; ++index){
                migrants[source].push_back(islands[source][order[index]]);
                migrant_scores[source].push_back(scores[source][order[index]]);
            }
        }
        for(std::size_t source = 0; source < islands.size(); ++source){
            const std::size_t destination = source + 1 == islands.size() ? 0 : source + 1;
            const auto destination_order = genetic_algorithm_detail::sorted_indices(
                scores[destination], compare
            );
            for(std::size_t index = 0; index < migrants_per_epoch; ++index){
                const std::size_t replaced = destination_order[
                    destination_order.size() - 1 - index
                ];
                islands[destination][replaced] = migrants[source][index];
                scores[destination][replaced] = migrant_scores[source][index];
            }
        }
    }

    std::size_t best_island = 0;
    std::size_t best_index = 0;
    for(std::size_t island_index = 0; island_index < islands.size(); ++island_index){
        for(std::size_t index = 0; index < islands[island_index].size(); ++index){
            if(std::invoke(
                compare, scores[island_index][index],
                scores[best_island][best_index]
            )){
                best_island = island_index;
                best_index = index;
            }
        }
    }
    return EvolutionResult<Individual, Score>{
        islands[best_island][best_index], scores[best_island][best_index],
        local_generations, expected_evaluations
    };
}

}  // namespace approximate::evolutionary
