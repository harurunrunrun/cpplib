// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/routing/insertion_heuristics.hpp"
#include "../../src/approximate/routing/tour_cost.hpp"

int main(){
    using namespace approximate::routing;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const std::vector<std::vector<int>> small_distance{
        {0, 2, 9},
        {2, 0, 3},
        {9, 3, 0},
    };
    assert(arbitrary_insertion_tour(small_distance).size() == 3);
    bool rejected = false;
    try{
        static_cast<void>(arbitrary_insertion_tour(
            small_distance,
            std::vector<int>{0, 1}
        ));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try{
        static_cast<void>(arbitrary_insertion_tour(
            small_distance,
            std::vector<int>{0, 1, 1}
        ));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try{
        static_cast<void>(arbitrary_insertion_tour(
            small_distance,
            std::vector<int>{0, 1, 3}
        ));
    }catch(const std::out_of_range&){
        rejected = true;
    }
    assert(rejected);

    int query_count = 0;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::size_t size = 0;
        std::cin >> size;
        std::vector<int> insertion_order(size);
        for(int& vertex : insertion_order) std::cin >> vertex;
        std::vector<std::vector<long long>> distance(
            size,
            std::vector<long long>(size)
        );
        for(auto& row : distance){
            for(long long& value : row) std::cin >> value;
        }
        const std::vector<int> tour = arbitrary_insertion_tour(
            distance,
            insertion_order
        );
        std::cout << tour.size();
        for(const int vertex : tour) std::cout << ' ' << vertex;
        std::cout << " | " << tour_cost(distance, tour) << '\n';
    }
}
