// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/approximate/evolutionary/compact_genetic_algorithm.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        std::string target;
        std::cin >> target;
        const auto evaluate = [&target](const std::vector<std::uint8_t>& bits){
            std::size_t equal = 0;
            for(std::size_t index = 0; index < bits.size(); ++index){
                equal += bits[index] == static_cast<std::uint8_t>(target[index] - '0');
            }
            return equal;
        };
        std::mt19937_64 first_random(2026071308ULL);
        std::mt19937_64 second_random(2026071308ULL);
        const auto first = approximate::evolutionary::
            compact_genetic_algorithm<double>(
                target.size(), evaluate, 3000, 80, first_random,
                std::greater<>{}
            );
        const auto repeated = approximate::evolutionary::
            compact_genetic_algorithm<double>(
                target.size(), evaluate, 3000, 80, second_random,
                std::greater<>{}
            );
        assert(first.individual == repeated.individual);
        assert(first.score == repeated.score);
        assert(first.probabilities == repeated.probabilities);
        assert(first.score == target.size());
        assert(first.evaluations == 6000U);
        for(const double probability: first.probabilities){
            assert(1.0 / 80.0 <= probability);
            assert(probability <= 79.0 / 80.0);
        }
    }

    bool thrown = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::evolutionary::
            compact_genetic_algorithm<double>(
                0, [](const auto&){ return 0; }, 1, 10, random
            ));
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    std::cout << "OK\n";
}
