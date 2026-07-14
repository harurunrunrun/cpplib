#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

class LcmSumTable{
    std::vector<std::uint32_t> smallest_prime;

public:
    explicit LcmSumTable(int limit):
        smallest_prime(static_cast<std::size_t>(limit) + 1)
    {
        if(limit < 1)[[unlikely]]{
            throw std::invalid_argument("LcmSumTable requires a positive limit");
        }
        for(int value = 2; value <= limit; ++value){
            if(smallest_prime[static_cast<std::size_t>(value)] != 0) continue;
            smallest_prime[static_cast<std::size_t>(value)] =
                static_cast<std::uint32_t>(value);
            if(value > limit / value) continue;
            for(int multiple = value * value; multiple <= limit; multiple += value){
                auto& entry = smallest_prime[static_cast<std::size_t>(multiple)];
                if(entry == 0) entry = static_cast<std::uint32_t>(value);
            }
        }
    }

    int limit() const{
        return static_cast<int>(smallest_prime.size()) - 1;
    }

    // Returns sum_{i=1}^n lcm(i,n).
    std::uint64_t value(int n) const{
        if(n < 1 || limit() < n)[[unlikely]]{
            throw std::out_of_range("LcmSumTable::value is outside the table");
        }
        int remaining = n;
        unsigned __int128 divisor_sum = 1;
        while(remaining > 1){
            const std::uint64_t p = smallest_prime[static_cast<std::size_t>(remaining)];
            std::uint64_t power = 1;
            unsigned __int128 local = 1;
            do{
                remaining /= static_cast<int>(p);
                power *= p;
                local += static_cast<unsigned __int128>(power) * (power - power / p);
            }while(remaining > 1 &&
                smallest_prime[static_cast<std::size_t>(remaining)] == p);
            divisor_sum *= local;
        }
        const unsigned __int128 product =
            static_cast<unsigned __int128>(n) * (divisor_sum + 1);
        return static_cast<std::uint64_t>(product / 2);
    }
};

} // namespace math
