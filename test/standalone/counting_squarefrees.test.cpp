// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/counting_squarefrees.hpp"

namespace{

std::vector<std::uint64_t> naive_prefix(std::size_t limit){
    std::vector<bool> squarefree(limit + 1, true);
    squarefree[0] = false;
    for(std::size_t prime = 2; prime <= limit / prime; ++prime){
        const std::size_t square = prime * prime;
        for(std::size_t multiple = square;
            multiple <= limit;
            multiple += square){
            squarefree[multiple] = false;
        }
    }
    std::vector<std::uint64_t> result(limit + 1);
    for(std::size_t value = 1; value <= limit; ++value){
        result[value] = result[value - 1] + (squarefree[value] ? 1U : 0U);
    }
    return result;
}

void self_test(){
    constexpr std::size_t limit = 200000;
    const auto expected = naive_prefix(limit);
    for(std::size_t value = 0; value <= 1000; ++value){
        assert(math::count_squarefree_numbers(value) == expected[value]);
        assert(math::count_squarefree(value) == expected[value]);
    }

    std::mt19937_64 engine(0x9216d5d98979fb1bULL);
    for(int iteration = 0; iteration < 500; ++iteration){
        const std::size_t value = engine() % (limit + 1);
        assert(math::count_squarefree(value) == expected[value]);
    }

    assert(math::count_squarefree(1'000'000'000'000ULL) == 607927102274ULL);

    bool thrown = false;
    try{
        (void)math::count_squarefree(
            math::counting_squarefrees_max_n + 1
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_test();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- != 0){
        std::uint64_t n;
        std::cin >> n;
        std::cout << math::count_squarefree_numbers(n) << '\n';
    }
}
