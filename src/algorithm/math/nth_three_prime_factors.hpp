#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace math{

class ThreeDistinctPrimeFactorsTable{
    std::vector<int> values;

public:
    explicit ThreeDistinctPrimeFactorsTable(int maximum_rank){
        if(maximum_rank < 0)[[unlikely]]{
            throw std::invalid_argument("maximum rank must be non-negative");
        }
        if(maximum_rank == 0) return;
        int limit = std::max(64, maximum_rank * 4);
        while(true){
            std::vector<unsigned char> distinct_count(
                static_cast<std::size_t>(limit) + 1
            );
            for(int prime = 2; prime <= limit; ++prime){
                if(distinct_count[static_cast<std::size_t>(prime)] != 0) continue;
                for(int multiple = prime; multiple <= limit; multiple += prime){
                    ++distinct_count[static_cast<std::size_t>(multiple)];
                }
            }
            values.clear();
            for(int value = 2; value <= limit; ++value){
                if(distinct_count[static_cast<std::size_t>(value)] >= 3){
                    values.push_back(value);
                }
            }
            if(static_cast<int>(values.size()) >= maximum_rank){
                values.resize(static_cast<std::size_t>(maximum_rank));
                return;
            }
            if(limit > 1'000'000'000 / 2)[[unlikely]]{
                throw std::overflow_error("rank is too large");
            }
            limit *= 2;
        }
    }

    int size() const{ return static_cast<int>(values.size()); }

    int value(int one_based_rank) const{
        if(one_based_rank < 1 || size() < one_based_rank)[[unlikely]]{
            throw std::out_of_range("rank is outside ThreeDistinctPrimeFactorsTable");
        }
        return values[static_cast<std::size_t>(one_based_rank - 1)];
    }
};

} // namespace math
