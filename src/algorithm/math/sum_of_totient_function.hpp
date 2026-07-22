#ifndef CPPLIB_SRC_ALGORITHM_MATH_SUM_OF_TOTIENT_FUNCTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_SUM_OF_TOTIENT_FUNCTION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "../../structure/modint/modint.hpp"

namespace math{

inline constexpr std::uint64_t sum_totient_function_max_n =
    10'000'000'000ULL;

namespace sum_totient_function_internal{

template<int MOD>
class Calculator{
    using mint = Modint<MOD>;

    std::uint64_t n_;
    std::size_t sieve_limit_;
    std::vector<std::uint32_t> prefix_;
    std::unordered_map<std::uint64_t, int> memo_;

    static mint triangular(std::uint64_t value){
        std::uint64_t left = value;
        std::uint64_t right = value + 1;
        if((left & 1U) == 0) left /= 2;
        else right /= 2;
        return mint(static_cast<long long>(left % MOD)) *
            mint(static_cast<long long>(right % MOD));
    }

    void build_prefix(){
        prefix_.assign(sieve_limit_ + 1, 0);
        if(sieve_limit_ == 0) return;

        std::vector<bool> composite(sieve_limit_ + 1, false);
        std::vector<std::uint32_t> primes;
        prefix_[1] = 1;
        for(std::size_t value = 2; value <= sieve_limit_; ++value){
            if(!composite[value]){
                primes.push_back(static_cast<std::uint32_t>(value));
                prefix_[value] = static_cast<std::uint32_t>(value - 1);
            }
            for(const std::uint32_t prime: primes){
                const std::uint64_t product =
                    static_cast<std::uint64_t>(value) * prime;
                if(product > sieve_limit_) break;
                composite[static_cast<std::size_t>(product)] = true;
                if(value % prime == 0){
                    prefix_[static_cast<std::size_t>(product)] =
                        prefix_[value] * prime;
                    break;
                }
                prefix_[static_cast<std::size_t>(product)] =
                    prefix_[value] * (prime - 1);
            }
        }

        std::uint64_t sum = 0;
        for(std::size_t value = 1; value <= sieve_limit_; ++value){
            sum += prefix_[value];
            sum %= MOD;
            prefix_[value] = static_cast<std::uint32_t>(sum);
        }
    }

    mint query(std::uint64_t value){
        if(value <= sieve_limit_){
            return mint(prefix_[static_cast<std::size_t>(value)]);
        }
        const auto found = memo_.find(value);
        if(found != memo_.end()) return mint(found->second);

        mint result = triangular(value);
        for(std::uint64_t left = 2; left <= value;){
            const std::uint64_t quotient = value / left;
            const std::uint64_t right = value / quotient;
            result -= mint(static_cast<long long>((right - left + 1) % MOD)) *
                query(quotient);
            left = right + 1;
        }
        memo_.emplace(value, result.val());
        return result;
    }

public:
    explicit Calculator(std::uint64_t n):
        n_(n),
        sieve_limit_(static_cast<std::size_t>(std::max<long double>(
            1.0L,
            std::pow(static_cast<long double>(n), 2.0L / 3.0L)
        )))
    {
        if(sieve_limit_ > n_) sieve_limit_ = static_cast<std::size_t>(n_);
        build_prefix();
        const std::uint64_t state_count = n_ / sieve_limit_;
        memo_.reserve(static_cast<std::size_t>(state_count * 2 + 1));
    }

    mint calculate(){
        return query(n_);
    }
};

} // namespace sum_totient_function_internal

template<int MOD = 998244353>
Modint<MOD> sum_of_totient_function(std::uint64_t n){
    static_assert(MOD >= 1);
    if(n == 0) return Modint<MOD>(0);
    if(n > sum_totient_function_max_n)[[unlikely]]{
        throw std::invalid_argument(
            "sum_of_totient_function: n exceeds sum_totient_function_max_n"
        );
    }
    return sum_totient_function_internal::Calculator<MOD>(n).calculate();
}

template<int MOD = 998244353>
Modint<MOD> sum_totient(std::uint64_t n){
    return sum_of_totient_function<MOD>(n);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_SUM_OF_TOTIENT_FUNCTION_HPP_INCLUDED
