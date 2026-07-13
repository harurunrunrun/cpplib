// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/metaheuristic/tabu_search.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    std::cin >> case_count;
    while(case_count-- > 0){
        int size;
        int initial;
        std::cin >> size >> initial;
        std::vector<int> score(static_cast<std::size_t>(size));
        for(int& value: score) std::cin >> value;
        const auto neighborhood = [size](int state){
            return std::vector<int>{
                (state + size - 1) % size,
                (state + 1) % size
            };
        };
        const auto evaluate = [&score](int state){
            return score[static_cast<std::size_t>(state)];
        };
        const auto result = approximate::metaheuristic::tabu_search(
            initial, neighborhood, evaluate,
            [](const int& state){ return state; },
            static_cast<std::size_t>(size), 2
        );
        std::cout << result.score << '\n';
    }

    bool thrown = false;
    try{
        static_cast<void>(approximate::metaheuristic::tabu_search(
            0,
            [](int){ return std::vector<int>{0}; },
            [](int value){ return value; },
            [](const int& value){ return value; },
            1, 0
        ));
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}
