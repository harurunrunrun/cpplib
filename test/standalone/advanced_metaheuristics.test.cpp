// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/metaheuristic/acceptance_search.hpp"
#include "../../src/approximate/metaheuristic/adaptive_acceptance_search.hpp"
#include "../../src/approximate/metaheuristic/adaptive_constructive_search.hpp"
#include "../../src/approximate/metaheuristic/advanced_neighborhood_search.hpp"
#include "../../src/approximate/metaheuristic/hyper_heuristic.hpp"
#include "../../src/approximate/metaheuristic/iterated_search.hpp"
#include "../../src/approximate/metaheuristic/tabu_search.hpp"

namespace advanced_metaheuristics_test {

using Engine = std::mt19937_64;
using UnaryOperator = std::function<long long(const long long&, Engine&)>;

std::vector<long long> adjacent(
    long long state,
    long long lower,
    long long upper
){
    std::vector<long long> result;
    if(state > lower) result.push_back(state - 1);
    if(state < upper) result.push_back(state + 1);
    return result;
}

std::vector<long long> scores_for_case(
    long long initial,
    long long target,
    long long lower,
    long long upper,
    Engine& random
){
    const auto evaluate = [target](long long state){
        return std::llabs(state - target);
    };
    const auto direct = [target](const long long&, Engine&){
        return target;
    };
    const std::vector<UnaryOperator> operators{direct, direct};
    const auto identity = [](long long state){ return state; };

    const auto skewed =
        approximate::metaheuristic::skewed_variable_neighborhood_search(
            initial, operators, identity, evaluate,
            [](long long lhs, long long rhs){ return std::llabs(lhs - rhs); },
            3, 0.25L, random
        );

    const auto guided = approximate::metaheuristic::guided_local_search(
        initial,
        [target](long long){ return std::vector<long long>{target}; },
        evaluate,
        [](const long long& state){
            return std::vector<std::pair<int, long double>>{
                {static_cast<int>(state & 7LL), 1.0L}
            };
        },
        3, 1.0L
    );

    const auto greedy = approximate::metaheuristic::iterated_greedy(
        initial,
        [target](long long, Engine&){ return target; },
        [](long long partial, Engine&){ return partial; },
        evaluate, 3, random
    );

    const auto step = approximate::metaheuristic::
        step_counting_hill_climbing(
            initial, direct, evaluate, 3, 1, random
        );

    const auto oscillation =
        approximate::metaheuristic::strategic_oscillation(
            initial,
            [target](long long){ return std::vector<long long>{target}; },
            evaluate,
            [lower, upper](long long state){
                if(state < lower) return lower - state;
                if(state > upper) return state - upper;
                return 0LL;
            },
            3, 1.0L, 0.5L
        );

    const auto extremal = approximate::metaheuristic::extremal_optimization(
        initial,
        [](const long long&){ return std::vector<std::size_t>{0, 1}; },
        [](const long long&, std::size_t component){
            return static_cast<long double>(component);
        },
        [target](const long long&, std::size_t, Engine&){ return target; },
        evaluate, 3, 1.5L, random
    );

    const auto adaptive_annealing =
        approximate::metaheuristic::adaptive_simulated_annealing(
            initial, direct, evaluate, 4, 2.0L, 2, 0.5L, 1.5L, random
        );

    const auto adaptive_random =
        approximate::metaheuristic::adaptive_random_search(
            [target](long double, Engine&){ return target; },
            evaluate, 4, 1.0L, 2.0L, 0.5L, 2, random
        );

    const auto parallel = approximate::metaheuristic::parallel_tempering(
        std::vector<long long>{initial, initial},
        std::vector<long double>{1.0L, 3.0L},
        direct, evaluate, 3, 1, random
    );

    const auto tempering = approximate::metaheuristic::simulated_tempering(
        initial, std::vector<long double>{1.0L, 2.0L},
        direct, evaluate, 4, 0.0L, random
    );

    const auto neighborhood = [lower, upper, target](long long state){
        std::vector<long long> result = adjacent(state, lower, upper);
        result.push_back(target);
        return result;
    };
    const auto key = [](const long long& state){ return state; };
    const auto reactive_tabu =
        approximate::metaheuristic::reactive_tabu_search(
            initial, neighborhood, evaluate, key,
            3, 2, 1, 4, 2
        );
    const auto granular_tabu =
        approximate::metaheuristic::granular_tabu_search(
            initial, neighborhood, evaluate, key,
            [](long long, long long){ return true; },
            3, 2
        );

    const auto reactive_grasp = approximate::metaheuristic::reactive_grasp(
        [target](long double, Engine&){ return target; },
        identity, evaluate,
        std::vector<long double>{0.0L, 0.5L, 1.0L},
        4, 2, random
    );

    const auto lns = approximate::metaheuristic::large_neighborhood_search(
        initial,
        [target](long long, Engine&){ return target; },
        [](long long partial, Engine&){ return partial; },
        evaluate,
        [](long long candidate, long long current, std::size_t, Engine&){
            return candidate <= current;
        },
        3, random
    );

    using Destroy = std::function<long long(const long long&, Engine&)>;
    using Repair = std::function<long long(long long, Engine&)>;
    const std::vector<Destroy> destroys{
        [target](const long long&, Engine&){ return target; },
        [target](const long long&, Engine&){ return target; }
    };
    const std::vector<Repair> repairs{
        [](long long partial, Engine&){ return partial; },
        [](long long partial, Engine&){ return partial; }
    };
    const auto adaptive_lns =
        approximate::metaheuristic::adaptive_large_neighborhood_search(
            initial, destroys, repairs, evaluate, 4, 0.25L, random
        );

    const auto ruin = approximate::metaheuristic::ruin_and_recreate(
        initial,
        [target](long long, std::size_t, Engine&){ return target; },
        [](long long partial, Engine&){ return partial; },
        evaluate, 4, 1, 3, random
    );

    const auto scatter = approximate::metaheuristic::scatter_search(
        std::vector<long long>{initial, lower, upper, target},
        [target](const long long&, const long long&, Engine&){
            return target;
        },
        identity, evaluate, 2, 4, random
    );

    const auto pilot = approximate::metaheuristic::pilot_method(
        initial,
        [target](long long state){ return state == target; },
        [target](long long){ return std::vector<long long>{target}; },
        [target](long long){ return target; },
        evaluate, 3
    );

    const auto cross_entropy =
        approximate::metaheuristic::cross_entropy_method(
            0.0L,
            [target](const long double&, Engine&){ return target; },
            evaluate,
            [](const std::vector<long long>&, long double parameters){
                return parameters;
            },
            3, 5, 2, random
        );

    const auto hyper = approximate::metaheuristic::hyper_heuristic(
        initial, operators, evaluate, 4, random
    );
    const auto choice =
        approximate::metaheuristic::choice_function_hyper_heuristic(
            initial, operators, evaluate, 4,
            0.5L, 0.1L, 0.1L, random
        );

    return {
        skewed.score,
        guided.score,
        greedy.score,
        step.score,
        oscillation.score,
        extremal.score,
        adaptive_annealing.score,
        adaptive_random.score,
        parallel.score,
        tempering.score,
        reactive_tabu.score,
        granular_tabu.score,
        reactive_grasp.score,
        lns.score,
        adaptive_lns.score,
        ruin.score,
        scatter.score,
        pilot.score,
        cross_entropy.score,
        hyper.score,
        choice.score
    };
}

void edge_case_tests(){
    Engine random(123);
    const auto evaluate = [](long long value){ return std::llabs(value); };
    const auto neighbor = [](const long long& state, Engine&){ return state; };
    bool threw = false;
    try{
        static_cast<void>(
            approximate::metaheuristic::adaptive_simulated_annealing(
                1LL, neighbor, evaluate, 1, 1.0L, 0, 0.5L, 2.0L, random
            )
        );
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);

    threw = false;
    try{
        const std::vector<UnaryOperator> empty;
        static_cast<void>(approximate::metaheuristic::hyper_heuristic(
            1LL, empty, evaluate, 1, random
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);

    threw = false;
    try{
        static_cast<void>(
            approximate::metaheuristic::cross_entropy_method(
                0,
                [](const int&, Engine&){ return 0LL; },
                evaluate,
                [](const std::vector<long long>&, int value){ return value; },
                0, 2, 1, random
            )
        );
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

}  // namespace advanced_metaheuristics_test

int main(){
    using namespace advanced_metaheuristics_test;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    edge_case_tests();

    std::size_t case_count;
    if(!(std::cin >> case_count)) return 0;
    Engine random(0xA4D4A11ULL);
    while(case_count-- > 0){
        long long initial;
        long long target;
        long long lower;
        long long upper;
        std::cin >> initial >> target >> lower >> upper;
        const auto scores = scores_for_case(
            initial, target, lower, upper, random
        );
        for(std::size_t i = 0; i < scores.size(); ++i){
            if(i != 0) std::cout << ' ';
            std::cout << scores[i];
        }
        std::cout << '\n';
    }
}
