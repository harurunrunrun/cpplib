// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/metaheuristic/hill_climbing.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    std::cin >> case_count;
    std::mt19937_64 random(2026071310ULL);
    while(case_count-- > 0){
        long long initial;
        long long target;
        long long lower;
        long long upper;
        std::cin >> initial >> target >> lower >> upper;
        const auto evaluate = [target](long long state){
            return std::llabs(state - target);
        };
        const auto neighborhood = [lower, upper](long long state){
            std::vector<long long> result;
            if(state > lower) result.push_back(state - 1);
            if(state < upper) result.push_back(state + 1);
            return result;
        };
        const std::size_t limit = static_cast<std::size_t>(upper - lower) + 2;
        const auto best = approximate::metaheuristic::
            best_improvement_hill_climbing(
                initial, neighborhood, evaluate, limit
            );
        const auto first = approximate::metaheuristic::
            first_improvement_hill_climbing(
                initial, neighborhood, evaluate, limit
            );
        const auto random_neighbor = [target](long long state, auto&){
            if(state < target) return state + 1;
            if(state > target) return state - 1;
            return state;
        };
        const auto stochastic = approximate::metaheuristic::
            stochastic_hill_climbing(
                initial, random_neighbor, evaluate, limit, random
            );
        long long next = lower;
        const auto generator = [&next, lower, upper](auto&){
            const long long result = next;
            next = next == upper ? lower : next + 1;
            return result;
        };
        const auto sampled = approximate::metaheuristic::random_search(
            generator, evaluate,
            static_cast<std::size_t>(upper - lower) + 1,
            random
        );
        const auto restart_generator = [initial](auto&){ return initial; };
        const auto restarted = approximate::metaheuristic::
            random_restart_hill_climbing(
                restart_generator, neighborhood, evaluate, 3, limit, random
            );
        std::cout << best.score << ' ' << first.score << ' '
                  << stochastic.score << ' ' << sampled.score << ' '
                  << restarted.score << '\n';
    }

    bool thrown = false;
    try{
        const auto generator = [](auto&){ return 0; };
        static_cast<void>(approximate::metaheuristic::random_search(
            generator, [](int value){ return value; }, 0, random
        ));
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}
