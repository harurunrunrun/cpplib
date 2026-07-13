// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>

#include "../../src/approximate/metaheuristic/acceptance_search.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    std::cin >> case_count;
    std::mt19937_64 random(2026071312ULL);
    while(case_count-- > 0){
        long long initial;
        long long target;
        std::size_t iterations;
        std::cin >> initial >> target >> iterations;
        const auto evaluate = [target](long long state){
            return std::llabs(state - target);
        };
        const auto neighbor = [target](long long state, auto&){
            if(state < target) return state + 1;
            if(state > target) return state - 1;
            return state;
        };
        const auto annealed = approximate::metaheuristic::simulated_annealing(
            initial, neighbor, evaluate, iterations, 10.0L, 0.99L, random
        );
        const auto fast = approximate::metaheuristic::fast_simulated_annealing(
            initial, neighbor, evaluate, iterations, 10.0L, random
        );
        const auto threshold = approximate::metaheuristic::threshold_accepting(
            initial, neighbor, evaluate, iterations, 3.0L, 0.95L, random
        );
        const auto deluge = approximate::metaheuristic::great_deluge(
            initial, neighbor, evaluate, iterations,
            static_cast<long double>(evaluate(initial)), 0.0L, random
        );
        const auto late = approximate::metaheuristic::
            late_acceptance_hill_climbing(
                initial, neighbor, evaluate, iterations, 8, random
            );
        std::cout << annealed.score << ' ' << fast.score << ' '
                  << threshold.score << ' ' << deluge.score << ' '
                  << late.score << '\n';
    }

    bool thrown = false;
    try{
        const auto neighbor = [](int value, auto&){ return value; };
        static_cast<void>(approximate::metaheuristic::simulated_annealing(
            0, neighbor, [](int value){ return value; },
            1, 0.0L, 0.9L, random
        ));
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}
