#ifndef CPPLIB_SRC_ALGORITHM_MATH_COUNTING_SQUAREFREES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COUNTING_SQUAREFREES_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

inline constexpr std::uint64_t counting_squarefrees_max_n =
    1'000'000'000'000'000'000ULL;

namespace counting_squarefrees_internal{

inline bool power_leq(
    std::uint64_t base,
    int exponent,
    std::uint64_t limit
){
    std::uint64_t value = 1;
    for(int count = 0; count < exponent; ++count){
        if(base != 0 && value > limit / base) return false;
        value *= base;
    }
    return value <= limit;
}

inline std::uint64_t integer_fifth_root(std::uint64_t value){
    if(value == 0) return 0;
    std::uint64_t result = static_cast<std::uint64_t>(
        std::pow(static_cast<long double>(value), 0.2L)
    );
    while(power_leq(result + 1, 5, value)) ++result;
    while(!power_leq(result, 5, value)) --result;
    return result;
}

inline std::uint64_t integer_square_root(std::uint64_t value){
    if(value == 0) return 0;
    std::uint64_t result = static_cast<std::uint64_t>(
        std::sqrt(static_cast<long double>(value))
    );
    while(result + 1 <= value / (result + 1)) ++result;
    while(result > value / result) --result;
    return result;
}

struct MobiusPrefix{
    std::vector<std::int8_t> mobius;
    std::vector<std::int32_t> mertens;

    explicit MobiusPrefix(std::size_t limit):
        mobius(limit + 1, 1),
        mertens(limit + 1)
    {
        mobius[0] = 0;
        std::vector<bool> composite(limit + 1, false);
        for(std::size_t prime = 2; prime <= limit; ++prime){
            if(composite[prime]) continue;
            if(prime <= limit / prime){
                for(std::size_t multiple = prime * prime;
                    multiple <= limit;
                    multiple += prime){
                    composite[multiple] = true;
                }
            }
            for(std::size_t multiple = prime;
                multiple <= limit;
                multiple += prime){
                mobius[multiple] = static_cast<std::int8_t>(-mobius[multiple]);
            }
            if(prime <= limit / prime){
                const std::size_t square = prime * prime;
                for(std::size_t multiple = square;
                    multiple <= limit;
                    multiple += square){
                    mobius[multiple] = 0;
                }
            }
        }

        std::int32_t sum = 0;
        for(std::size_t value = 1; value <= limit; ++value){
            sum += mobius[value];
            mertens[value] = sum;
        }
    }
};

} // namespace counting_squarefrees_internal

inline std::uint64_t count_squarefree_numbers(std::uint64_t n){
    if(n > counting_squarefrees_max_n)[[unlikely]]{
        throw std::invalid_argument("count_squarefree_numbers supports n <= 10^18");
    }
    if(n == 0) return 0;

    using namespace counting_squarefrees_internal;
    const std::uint64_t split_count = std::max<std::uint64_t>(
        1, integer_fifth_root(n)
    );
    const std::uint64_t sieve_limit = integer_square_root(n / split_count);
    MobiusPrefix prefix(static_cast<std::size_t>(sieve_limit));

    std::int64_t small_part = 0;
    for(std::uint64_t divisor = 1; divisor <= sieve_limit; ++divisor){
        const std::uint64_t square = divisor * divisor;
        small_part += static_cast<std::int64_t>(prefix.mobius[divisor]) *
            static_cast<std::int64_t>(n / square);
    }

    std::vector<std::int64_t> large_mertens(
        static_cast<std::size_t>(split_count)
    );
    std::int64_t large_mertens_sum = 0;
    for(std::uint64_t index = split_count; index-- > 1;){
        const std::uint64_t value = integer_square_root(n / index);
        std::int64_t mertens_value = 1;
        for(std::uint64_t left = 2; left <= value;){
            const std::uint64_t quotient = value / left;
            const std::uint64_t right = value / quotient;
            std::int64_t quotient_mertens;
            if(quotient <= sieve_limit){
                quotient_mertens = prefix.mertens[quotient];
            }else{
                const std::uint64_t mapped_index = index * left * left;
                if(mapped_index >= split_count)[[unlikely]]{
                    throw std::logic_error("invalid squarefree Mertens mapping");
                }
                quotient_mertens = large_mertens[mapped_index];
            }
            mertens_value -= static_cast<std::int64_t>(right - left + 1) *
                quotient_mertens;
            left = right + 1;
        }
        large_mertens[index] = mertens_value;
        large_mertens_sum += mertens_value;
    }

    const std::int64_t large_part = large_mertens_sum -
        static_cast<std::int64_t>(split_count - 1) *
        prefix.mertens[sieve_limit];
    const std::int64_t answer = small_part + large_part;
    if(answer < 0 || static_cast<std::uint64_t>(answer) > n)[[unlikely]]{
        throw std::logic_error("invalid squarefree count");
    }
    return static_cast<std::uint64_t>(answer);
}

inline std::uint64_t count_squarefree(std::uint64_t n){
    return count_squarefree_numbers(n);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COUNTING_SQUAREFREES_HPP_INCLUDED
