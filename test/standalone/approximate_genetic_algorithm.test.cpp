// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/evolutionary/genetic_algorithm.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        long long target;
        std::size_t population_size;
        std::cin >> target >> population_size;
        std::vector<long long> population(population_size);
        for(auto& value: population) std::cin >> value;
        const auto evaluate = [target](long long value){
            return std::llabs(value - target);
        };
        const auto select = [](const auto& individuals, const auto&, auto& random){
            std::uniform_int_distribution<std::size_t> distribution(
                0, individuals.size() - 1
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
        long long initial_best = evaluate(population.front());
        for(const long long value: population){
            initial_best = std::min(initial_best, evaluate(value));
        }

        std::mt19937_64 first_random(2026071301ULL);
        std::mt19937_64 second_random(2026071301ULL);
        const auto first = approximate::evolutionary::
            generational_genetic_algorithm(
                population, evaluate, select, crossover, mutate,
                60, first_random, 1
            );
        const auto repeated = approximate::evolutionary::
            generational_genetic_algorithm(
                population, evaluate, select, crossover, mutate,
                60, second_random, 1
            );
        assert(first.individual == repeated.individual);
        assert(first.score == repeated.score);
        assert(first.score <= initial_best);
        assert(first.iterations == 60U);
        assert(first.evaluations == population_size * 61U);

        std::mt19937_64 third_random(2026071302ULL);
        std::mt19937_64 fourth_random(2026071302ULL);
        const auto steady = approximate::evolutionary::
            steady_state_genetic_algorithm(
                population, evaluate, select, crossover, mutate,
                240, third_random
            );
        const auto steady_repeated = approximate::evolutionary::
            steady_state_genetic_algorithm(
                population, evaluate, select, crossover, mutate,
                240, fourth_random
            );
        assert(steady.individual == steady_repeated.individual);
        assert(steady.score == steady_repeated.score);
        assert(steady.score <= initial_best);
        assert(steady.evaluations == population_size + 240U);
    }

    bool empty_thrown = false;
    std::mt19937_64 random(1);
    try{
        std::vector<int> empty;
        const auto evaluate = [](int value){ return value; };
        const auto select = [](const auto&, const auto&, auto&){ return 0U; };
        const auto crossover = [](int lhs, int, auto&){ return lhs; };
        const auto mutate = [](int&, auto&){};
        static_cast<void>(approximate::evolutionary::
            generational_genetic_algorithm(
                empty, evaluate, select, crossover, mutate, 1, random
            ));
    }catch(const std::invalid_argument&){
        empty_thrown = true;
    }
    assert(empty_thrown);

    bool selector_thrown = false;
    try{
        std::vector<int> population{1, 2};
        const auto evaluate = [](int value){ return value; };
        const auto bad_select = [](const auto& values, const auto&, auto&){
            return values.size();
        };
        const auto crossover = [](int lhs, int, auto&){ return lhs; };
        const auto mutate = [](int&, auto&){};
        static_cast<void>(approximate::evolutionary::
            steady_state_genetic_algorithm(
                population, evaluate, bad_select, crossover, mutate, 1, random
            ));
    }catch(const std::out_of_range&){
        selector_thrown = true;
    }
    assert(selector_thrown);
    std::cout << "OK\n";
}
