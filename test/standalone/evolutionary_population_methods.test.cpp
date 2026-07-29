// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/evolutionary/ant_colony_optimization.hpp"
#include "../../src/approximate/evolutionary/cma_es.hpp"
#include "../../src/approximate/evolutionary/compact_genetic_algorithm.hpp"
#include "../../src/approximate/evolutionary/differential_evolution.hpp"
#include "../../src/approximate/evolutionary/distribution_estimation.hpp"
#include "../../src/approximate/evolutionary/evolution_strategy.hpp"
#include "../../src/approximate/evolutionary/genetic_algorithm.hpp"
#include "../../src/approximate/evolutionary/hybrid_genetic_algorithm.hpp"
#include "../../src/approximate/evolutionary/nature_inspired_optimization.hpp"
#include "../../src/approximate/evolutionary/particle_swarm_optimization.hpp"

namespace evolutionary_population_methods_test {

using Engine = std::mt19937_64;
using Point = std::vector<double>;
using Bits = std::vector<std::uint8_t>;

std::vector<long long> run_case(std::size_t dimension, Engine& random){
    const Point lower(dimension, -1.0);
    const Point upper(dimension, 1.0);
    const Point zero(dimension, 0.0);
    const std::vector<Point> population(5, zero);
    const auto constant_point = [](const Point&){ return 0LL; };

    const auto cma = approximate::evolutionary::cma_es(
        zero, lower, upper, constant_point, 3, 6, 0.2, random
    );

    const auto complete = [dimension](int state){
        return state == static_cast<int>(dimension);
    };
    const auto choices = [](const int&){
        return std::vector<int>{1};
    };
    const auto apply = [](int state, const int& move){
        return state + move;
    };
    const auto heuristic = [](const int&, const int&){ return 1.0; };
    const auto key = [](const int& state, const int&){
        return state;
    };
    const auto evaluate_path = [dimension](const int& state){
        return std::llabs(state - static_cast<long long>(dimension));
    };
    const auto generic_update = [](auto& pheromones, const auto&,
                                   const auto&, const auto&,
                                   std::size_t){
        for(auto& entry : pheromones) entry.second = 1.0;
    };
    const auto aco = approximate::evolutionary::ant_colony_optimization(
        0, complete, choices, apply, heuristic, key, evaluate_path,
        generic_update, 2, 3, dimension + 1, 1.0, 1.0, 1.0, random
    );
    const auto deposit = [](long long){ return 1.0; };
    const auto ant_system = approximate::evolutionary::ant_system(
        0, complete, choices, apply, heuristic, key, evaluate_path,
        deposit, 2, 3, dimension + 1,
        1.0, 0.2, 1.0, 1.0, random
    );
    const auto acs = approximate::evolutionary::ant_colony_system(
        0, complete, choices, apply, heuristic, key, evaluate_path,
        deposit, 2, 3, dimension + 1,
        1.0, 0.2, 0.1, 1.0, 1.0, random
    );
    const auto max_min = approximate::evolutionary::max_min_ant_system(
        0, complete, choices, apply, heuristic, key, evaluate_path,
        deposit, 2, 3, dimension + 1,
        1.0, 0.1, 2.0, 0.2, 1.0, 1.0, random
    );

    const auto abc = approximate::evolutionary::artificial_bee_colony(
        population, lower, upper, constant_point, 2, 2, random
    );
    const auto bees = approximate::evolutionary::bees_algorithm(
        population, lower, upper, constant_point,
        2, 3, 1, 2, 3, 0.2, 0.9, random
    );
    const auto harmony = approximate::evolutionary::harmony_search(
        population, lower, upper, constant_point,
        3, 0.9, 0.3, 0.1, random
    );
    const auto firefly = approximate::evolutionary::firefly_algorithm(
        population, lower, upper, constant_point,
        2, 1.0, 1.0, 0.1, random
    );
    const auto cuckoo = approximate::evolutionary::cuckoo_search(
        population, lower, upper, constant_point,
        2, 0.1, 0.2, random
    );
    const auto bat = approximate::evolutionary::bat_algorithm(
        population, lower, upper, constant_point,
        2, 0.0, 2.0, 0.9, 0.5, 0.9, 0.5, random
    );
    const auto wolf = approximate::evolutionary::grey_wolf_optimizer(
        population, lower, upper, constant_point, 2, random
    );

    const auto constant_bits = [](const Bits&){ return 0LL; };
    const auto eda =
        approximate::evolutionary::estimation_of_distribution_algorithm(
            std::vector<double>(dimension, 0.5),
            [dimension](const std::vector<double>&, Engine&){
                return Bits(dimension);
            },
            [](const std::vector<Bits>&, std::vector<double> parameters){
                return parameters;
            },
            constant_bits, 2, 5, 2, random
        );
    const auto umda =
        approximate::evolutionary::
            univariate_marginal_distribution_algorithm(
                dimension, constant_bits, 2, 5, 2, random, 0.05
            );
    const auto pbil =
        approximate::evolutionary::
            population_based_incremental_learning(
                dimension, constant_bits, 2, 5,
                0.2, 0.1, 0.05, random
            );

    assert(cma.evaluations == 18);
    assert(aco.evaluations == 6);
    assert(umda.evaluations == 10);
    return {
        cma.score, aco.score, ant_system.score, acs.score, max_min.score,
        abc.score, bees.score, harmony.score, firefly.score,
        cuckoo.score, bat.score, wolf.score, eda.score, umda.score,
        pbil.score
    };
}

void edge_case_tests(){
    Engine random(17);
    const auto evaluate = [](const Point&){ return 0; };
    bool threw = false;
    try{
        static_cast<void>(approximate::evolutionary::cma_es(
            Point{0.0}, Point{-1.0}, Point{1.0},
            evaluate, 0, 4, 0.1, random
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);

    threw = false;
    try{
        static_cast<void>(
            approximate::evolutionary::
                population_based_incremental_learning(
                    2, [](const Bits&){ return 0; }, 1, 2,
                    0.0, 0.0, 0.0, random
                )
        );
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

}  // namespace evolutionary_population_methods_test

int main(){
    using namespace evolutionary_population_methods_test;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    edge_case_tests();
    std::size_t case_count;
    if(!(std::cin >> case_count)) return 0;
    Engine random(0xE701A11ULL);
    while(case_count-- > 0){
        std::size_t dimension;
        std::cin >> dimension;
        const auto scores = run_case(dimension, random);
        for(std::size_t i = 0; i < scores.size(); ++i){
            if(i != 0) std::cout << ' ';
            std::cout << scores[i];
        }
        std::cout << '\n';
    }
}
