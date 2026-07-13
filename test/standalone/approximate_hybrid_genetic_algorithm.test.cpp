// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/evolutionary/hybrid_genetic_algorithm.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        long long target;
        std::cin >> target;
        std::vector<std::vector<long long>> islands(3, std::vector<long long>(8));
        for(auto& island: islands){
            for(auto& value: island) std::cin >> value;
        }
        const auto evaluate = [target](long long value){
            return std::llabs(value - target);
        };
        const auto select = [](const auto& population, const auto&, auto& random){
            std::uniform_int_distribution<std::size_t> distribution(
                0, population.size() - 1
            );
            return distribution(random);
        };
        const auto crossover = [](long long lhs, long long rhs, auto& random){
            std::bernoulli_distribution choose(0.5);
            return choose(random) ? lhs : rhs;
        };
        const auto mutate = [target](long long& value, auto&){
            if(value < target) ++value;
            else if(target < value) --value;
        };
        const auto local_improve = [target](long long& value, auto&){
            for(int step = 0; step < 3; ++step){
                if(value < target) ++value;
                else if(target < value) --value;
            }
        };
        long long initial_best = evaluate(islands.front().front());
        long long first_island_best = initial_best;
        for(const long long value: islands.front()){
            first_island_best = std::min(first_island_best, evaluate(value));
        }
        for(const auto& island: islands){
            for(const long long value: island){
                initial_best = std::min(initial_best, evaluate(value));
            }
        }

        std::mt19937_64 first_random(2026071309ULL);
        std::mt19937_64 second_random(2026071309ULL);
        const auto memetic = approximate::evolutionary::memetic_genetic_algorithm(
            islands.front(), evaluate, select, crossover, mutate, local_improve,
            30, first_random, 1
        );
        const auto memetic_repeated = approximate::evolutionary::
            memetic_genetic_algorithm(
                islands.front(), evaluate, select, crossover, mutate,
                local_improve, 30, second_random, 1
            );
        assert(memetic.individual == memetic_repeated.individual);
        assert(memetic.score == memetic_repeated.score);
        assert(memetic.score <= first_island_best);
        assert(memetic.evaluations == 8U * 31U);

        std::mt19937_64 third_random(2026071310ULL);
        std::mt19937_64 fourth_random(2026071310ULL);
        const auto model = approximate::evolutionary::
            island_model_genetic_algorithm(
                islands, evaluate, select, crossover, mutate,
                10, 4, 2, third_random, 1
            );
        const auto model_repeated = approximate::evolutionary::
            island_model_genetic_algorithm(
                islands, evaluate, select, crossover, mutate,
                10, 4, 2, fourth_random, 1
            );
        assert(model.individual == model_repeated.individual);
        assert(model.score == model_repeated.score);
        assert(model.score <= initial_best);
        assert(model.evaluations == 24U * 41U);
    }

    bool thrown = false;
    try{
        std::mt19937_64 random(1);
        std::vector<std::vector<int>> islands{{1, 2}, {3, 4}};
        const auto select = [](const auto&, const auto&, auto&){ return 0U; };
        const auto crossover = [](int lhs, int, auto&){ return lhs; };
        const auto mutate = [](int&, auto&){};
        static_cast<void>(approximate::evolutionary::
            island_model_genetic_algorithm(
                islands, [](int value){ return value; }, select,
                crossover, mutate, 1, 1, 3, random
            ));
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    std::cout << "OK\n";
}
