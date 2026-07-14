#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

class SmallestPrimeFactorPrefixSum{
    std::vector<std::uint64_t> prefix;

public:
    explicit SmallestPrimeFactorPrefixSum(int limit):
        prefix(static_cast<std::size_t>(limit) + 1)
    {
        if(limit < 0)[[unlikely]]{
            throw std::invalid_argument("negative SPF prefix limit");
        }
        std::vector<std::uint32_t> smallest(static_cast<std::size_t>(limit) + 1);
        for(int value = 2; value <= limit; ++value){
            if(smallest[static_cast<std::size_t>(value)] == 0){
                smallest[static_cast<std::size_t>(value)] =
                    static_cast<std::uint32_t>(value);
                if(value <= limit / value){
                    for(int multiple = value * value; multiple <= limit;
                        multiple += value
                    ){
                        auto& entry = smallest[static_cast<std::size_t>(multiple)];
                        if(entry == 0) entry = static_cast<std::uint32_t>(value);
                    }
                }
            }
            prefix[static_cast<std::size_t>(value)] =
                prefix[static_cast<std::size_t>(value - 1)] +
                smallest[static_cast<std::size_t>(value)];
        }
    }

    int limit() const{ return static_cast<int>(prefix.size()) - 1; }

    std::uint64_t value(int n) const{
        if(n < 0 || limit() < n)[[unlikely]]{
            throw std::out_of_range("SPF prefix query is outside the table");
        }
        return prefix[static_cast<std::size_t>(n)];
    }
};

} // namespace math
