// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/other/minimize_maximum_partition_sum.hpp"

long long brute_partition_sum(
    const std::vector<long long>& values,
    std::size_t partition_count
){
    const std::size_t boundary_count = values.size() - 1;
    const unsigned long long mask_limit = 1ULL << boundary_count;
    long long answer = std::numeric_limits<long long>::max();
    for(unsigned long long mask = 0; mask < mask_limit; ++mask){
        std::size_t cuts = 0;
        for(std::size_t bit = 0; bit < boundary_count; ++bit){
            if(((mask >> bit) & 1ULL) != 0) ++cuts;
        }
        if(cuts + 1 != partition_count) continue;

        long long sum = 0;
        long long maximum = std::numeric_limits<long long>::lowest();
        for(std::size_t index = 0; index < values.size(); ++index){
            sum += values[index];
            if(index == boundary_count || ((mask >> index) & 1ULL) != 0){
                maximum = std::max(maximum, sum);
                sum = 0;
            }
        }
        answer = std::min(answer, maximum);
    }
    return answer;
}

void self_test(){
    assert(minimize_maximum_partition_sum(
        {1, 1, 1, 3, 2, 2, 1, 3, 1}, 4
    ) == 5);
    assert(minimize_maximum_partition_sum({10, -9, -9, 10}, 2) == 1);
    assert(minimize_maximum_partition_sum({-5, -2, -7}, 2) == -7);
    assert(minimize_maximum_partition_sum({4, -20, 7, 8, -3}, 3) == 4);
    assert(minimize_maximum_partition_sum({7}, 1) == 7);
    assert(minimize_maximum_partition_sum({3, -10, 4}, 1) == -3);
    assert(minimize_maximum_partition_sum({3, -10, 4}, 3) == 4);

    bool thrown = false;
    try{
        (void)minimize_maximum_partition_sum({}, 0);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    for(std::size_t invalid_count: {std::size_t{0}, std::size_t{4}}){
        thrown = false;
        try{
            (void)minimize_maximum_partition_sum({1, 2, 3}, invalid_count);
        }catch(const std::invalid_argument&){
            thrown = true;
        }
        assert(thrown);
    }

    thrown = false;
    try{
        (void)minimize_maximum_partition_sum(
            {
                std::numeric_limits<long long>::max(),
                std::numeric_limits<long long>::max()
            },
            1
        );
    }catch(const std::overflow_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)minimize_maximum_partition_sum(
            {
                std::numeric_limits<long long>::lowest(),
                std::numeric_limits<long long>::lowest()
            },
            1
        );
    }catch(const std::overflow_error&){
        thrown = true;
    }
    assert(thrown);
    assert(minimize_maximum_partition_sum(
        {
            std::numeric_limits<long long>::lowest(),
            std::numeric_limits<long long>::lowest()
        },
        2
    ) == std::numeric_limits<long long>::lowest());

    std::mt19937_64 random(2026071406);
    for(int iteration = 0; iteration < 4000; ++iteration){
        const std::size_t size = static_cast<std::size_t>(random() % 10 + 1);
        const std::size_t partition_count =
            static_cast<std::size_t>(random() % size + 1);
        std::vector<long long> values(size);
        for(long long& value: values){
            value = static_cast<long long>(random() % 41) - 20;
        }
        assert(minimize_maximum_partition_sum(values, partition_count)
            == brute_partition_sum(values, partition_count));
    }

    constexpr std::size_t large_size = 15000;
    std::vector<long long> large(large_size);
    for(std::size_t index = 0; index < large_size; ++index){
        large[index] = index % 2 == 0 ? 30000 : -30000;
    }
    assert(minimize_maximum_partition_sum(large, large_size / 2) == 0);
}

int main(){
    std::size_t size;
    std::size_t partition_count;
    if(!(std::cin >> size >> partition_count)){
        self_test();
        return 0;
    }
    std::vector<long long> values(size);
    for(long long& value: values) std::cin >> value;
    std::cout << minimize_maximum_partition_sum(values, partition_count) << '\n';
}
