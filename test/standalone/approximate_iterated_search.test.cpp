// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include "../../src/approximate/metaheuristic/iterated_search.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    std::cin >> case_count;
    std::mt19937_64 random(2026071314ULL);
    while(case_count-- > 0){
        long long initial;
        long long target;
        long long lower;
        long long upper;
        std::cin >> initial >> target >> lower >> upper;
        const auto evaluate = [target](long long state){
            return std::llabs(state - target);
        };
        const auto descend = [target](long long){ return target; };
        const auto perturb = [lower, upper](long long state, auto&){
            if(state < upper) return state + 1;
            if(state > lower) return state - 1;
            return state;
        };
        const auto ils = approximate::metaheuristic::iterated_local_search(
            initial, descend, perturb, evaluate, 4, random
        );

        using Neighborhood = std::function<
            std::vector<long long>(const long long&)
        >;
        const std::vector<Neighborhood> neighborhoods{
            [lower, upper](const long long& state){
                std::vector<long long> result;
                if(state > lower) result.push_back(state - 1);
                if(state < upper) result.push_back(state + 1);
                return result;
            },
            [lower, upper](const long long& state){
                std::vector<long long> result;
                if(state - lower >= 2) result.push_back(state - 2);
                if(upper - state >= 2) result.push_back(state + 2);
                return result;
            }
        };
        const auto vnd = approximate::metaheuristic::
            variable_neighborhood_descent(
                initial, neighborhoods, evaluate,
                static_cast<std::size_t>(upper - lower) + 2
            );

        using Shaking = std::function<
            long long(const long long&, std::mt19937_64&)
        >;
        const std::vector<Shaking> shakings{
            [target](const long long& state, std::mt19937_64&){
                if(state < target) return state + 1;
                if(state > target) return state - 1;
                return state;
            }
        };
        const auto identity = [](long long state){ return state; };
        const auto vns = approximate::metaheuristic::variable_neighborhood_search(
            initial, shakings, identity, evaluate,
            static_cast<std::size_t>(upper - lower) + 2, random
        );

        const auto construct = [lower, upper](auto& engine){
            std::uniform_int_distribution<long long> distribution(lower, upper);
            return distribution(engine);
        };
        const auto grasp = approximate::metaheuristic::grasp(
            construct, [target](long long){ return target; },
            evaluate, 5, random
        );
        const auto destroyed = approximate::metaheuristic::destroy_and_repair(
            initial,
            [target](long long, auto&){ return target; },
            [](long long partial, auto&){ return partial; },
            evaluate, 2, random
        );
        std::cout << ils.score << ' ' << vnd.score << ' ' << vns.score << ' '
                  << grasp.score << ' ' << destroyed.score << '\n';
    }
}
