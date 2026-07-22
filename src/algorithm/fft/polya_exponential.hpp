#ifndef CPPLIB_SRC_ALGORITHM_FFT_POLYA_EXPONENTIAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_POLYA_EXPONENTIAL_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "formal_power_series.hpp"

template<int MOD>
std::vector<Modint<MOD>> polya_exponential(
    const std::vector<Modint<MOD>>& values,
    const std::size_t size
){
    static_assert(MOD >= 2);
    if(size == 0) return {};
    if(size > static_cast<std::size_t>(MOD))[[unlikely]]{
        throw std::invalid_argument(
            "Polya exponential requires target size at most the modulus"
        );
    }
    if(!values.empty() && values[0] != Modint<MOD>(0))[[unlikely]]{
        throw std::invalid_argument(
            "Polya exponential requires constant term zero"
        );
    }
    std::vector<Modint<MOD>> exponent(size);
    for(std::size_t source_degree = 1;
        source_degree < values.size() && source_degree < size;
        ++source_degree){
        const std::size_t maximum_multiplier = (size - 1) / source_degree;
        for(std::size_t multiplier = 1;
            multiplier <= maximum_multiplier; ++multiplier){
            exponent[source_degree * multiplier] +=
                values[source_degree] /
                Modint<MOD>(static_cast<long long>(multiplier));
        }
    }
    return fps_exp<MOD>(exponent, size);
}

template<int MOD>
std::vector<Modint<MOD>> polya_exponential(
    const std::vector<Modint<MOD>>& values
){
    return polya_exponential<MOD>(values, values.size());
}

template<int MOD>
std::vector<Modint<MOD>> polya_logarithm(
    const std::vector<Modint<MOD>>& values,
    const std::size_t size
){
    static_assert(MOD >= 2);
    if(size == 0) return {};
    if(size > static_cast<std::size_t>(MOD))[[unlikely]]{
        throw std::invalid_argument(
            "Polya logarithm requires target size at most the modulus"
        );
    }
    if(values.empty() || values[0] != Modint<MOD>(1))[[unlikely]]{
        throw std::invalid_argument(
            "Polya logarithm requires constant term one"
        );
    }
    const std::vector<Modint<MOD>> logarithm = fps_log<MOD>(values, size);

    std::vector<int> mobius(size, 1);
    std::vector<int> primes;
    std::vector<int> least_prime(size);
    if(size > 0) mobius[0] = 0;
    for(std::size_t value = 2; value < size; ++value){
        if(least_prime[value] == 0){
            least_prime[value] = static_cast<int>(value);
            primes.push_back(static_cast<int>(value));
            mobius[value] = -1;
        }
        for(const int prime: primes){
            if(static_cast<std::size_t>(prime) >
               (size - 1) / value) break;
            const std::size_t product = value *
                static_cast<std::size_t>(prime);
            least_prime[product] = prime;
            if(value % static_cast<std::size_t>(prime) == 0){
                mobius[product] = 0;
                break;
            }
            mobius[product] = -mobius[value];
        }
    }

    std::vector<Modint<MOD>> result(size);
    for(std::size_t multiplier = 1; multiplier < size; ++multiplier){
        if(mobius[multiplier] == 0) continue;
        const Modint<MOD> scale = Modint<MOD>(mobius[multiplier]) /
            Modint<MOD>(static_cast<long long>(multiplier));
        const std::size_t maximum_degree = (size - 1) / multiplier;
        for(std::size_t source_degree = 1;
            source_degree <= maximum_degree; ++source_degree){
            result[source_degree * multiplier] +=
                logarithm[source_degree] * scale;
        }
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> polya_logarithm(
    const std::vector<Modint<MOD>>& values
){
    return polya_logarithm<MOD>(values, values.size());
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_POLYA_EXPONENTIAL_HPP_INCLUDED
