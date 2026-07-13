// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/set_cover/greedy_max_coverage.hpp"
#include "../../src/approximate/set_cover/greedy_set_cover.hpp"

namespace {

void print_vector(const std::vector<std::size_t>& values){
    std::cout << values.size();
    for(const std::size_t value : values) std::cout << ' ' << value;
}

}  // namespace

int main(){
    using namespace approximate::set_cover;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool rejected = false;
    try{
        static_cast<void>(greedy_set_cover(1, {{1}}));
    }catch(const std::out_of_range&){
        rejected = true;
    }
    if(!rejected) return 2;
    rejected = false;
    try{
        static_cast<void>(greedy_weighted_set_cover<long long>(1, {{0}}, {-1}));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 3;
    const long double maximum = std::numeric_limits<long double>::max();
    const SetCoverResult floating_boundary = greedy_weighted_set_cover<long double>(
        2,
        {{0, 1}, {0, 1}},
        {maximum, maximum * 0.75L}
    );
    if(floating_boundary.selected_sets != std::vector<std::size_t>{1}) return 4;

    std::size_t query_count = 0;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::size_t universe_size = 0;
        std::size_t set_count = 0;
        std::size_t budget = 0;
        std::cin >> universe_size >> set_count >> budget;
        std::vector<long long> weights(set_count);
        for(long long& weight : weights) std::cin >> weight;
        std::vector<std::vector<std::size_t>> sets(set_count);
        for(auto& set : sets){
            std::size_t size = 0;
            std::cin >> size;
            set.resize(size);
            for(std::size_t& element : set) std::cin >> element;
        }
        const SetCoverResult unweighted = greedy_set_cover(universe_size, sets);
        const SetCoverResult weighted = greedy_weighted_set_cover(universe_size, sets, weights);
        const MaxCoverageResult coverage = greedy_max_coverage(universe_size, sets, budget);
        print_vector(unweighted.selected_sets);
        std::cout << " | ";
        print_vector(unweighted.uncovered_elements);
        std::cout << " | ";
        print_vector(weighted.selected_sets);
        std::cout << " | ";
        print_vector(weighted.uncovered_elements);
        std::cout << " | ";
        print_vector(coverage.selected_sets);
        std::cout << " | ";
        print_vector(coverage.covered_elements);
        std::cout << " | ";
        print_vector(coverage.uncovered_elements);
        std::cout << '\n';
    }
}
