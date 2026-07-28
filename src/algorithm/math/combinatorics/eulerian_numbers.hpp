#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_EULERIAN_NUMBERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_EULERIAN_NUMBERS_HPP_INCLUDED

#include <bit>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "../../fft/transform/convolution.hpp"
#include "../../../structure/modint/modint.hpp"

namespace math{

namespace eulerian_numbers_internal{

template<int MOD>
inline constexpr bool directly_ntt_friendly =
    MOD == 167772161 || MOD == 469762049
    || MOD == 754974721 || MOD == 998244353;

}  // namespace eulerian_numbers_internal

template<int MOD>
std::vector<Modint<MOD>> eulerian_numbers(std::size_t order){
    if(order == 0) return {Modint<MOD>(1)};
    if constexpr(
        eulerian_numbers_internal::directly_ntt_friendly<MOD>
    ){
        const std::size_t maximum_transform =
            std::size_t{1} << std::countr_zero(
                static_cast<unsigned int>(MOD - 1)
            );
        if(order <= (maximum_transform - 2) / 2
            && order < static_cast<std::size_t>(MOD - 1)
            && order <= static_cast<std::size_t>(
                std::numeric_limits<long long>::max()
            )){
            using Mint = Modint<MOD>;
            std::vector<Mint> factorial(order + 2, Mint(1));
            for(std::size_t index = 1; index < factorial.size(); ++index){
                factorial[index] = factorial[index - 1]
                    * Mint(static_cast<long long>(index));
            }
            std::vector<Mint> inverse_factorial(order + 2);
            inverse_factorial.back() = factorial.back().inv();
            for(std::size_t index = order + 1; index > 0; --index){
                inverse_factorial[index - 1] = inverse_factorial[index]
                    * Mint(static_cast<long long>(index));
            }
            std::vector<Mint> signed_binomial(order + 2);
            for(std::size_t index = 0; index <= order + 1; ++index){
                signed_binomial[index] = factorial[order + 1]
                    * inverse_factorial[index]
                    * inverse_factorial[order + 1 - index];
                if((index & 1U) != 0){
                    signed_binomial[index] = -signed_binomial[index];
                }
            }
            std::vector<Mint> powers(order + 1);
            for(std::size_t value = 0; value <= order; ++value){
                powers[value] = Mint(static_cast<long long>(value)).pow(
                    static_cast<long long>(order)
                );
            }
            const std::vector<Mint> convolution =
                convolution_ntt<MOD>(
                    std::move(signed_binomial), std::move(powers)
                );
            std::vector<Mint> result(order);
            for(std::size_t descents = 0;
                descents < order;
                ++descents){
                result[descents] = convolution[descents + 1];
            }
            return result;
        }
    }
    std::vector<Modint<MOD>> previous(1, Modint<MOD>(1));
    for(std::size_t size = 2; size <= order; ++size){
        std::vector<Modint<MOD>> current(size);
        for(std::size_t descents = 0; descents < size; ++descents){
            if(descents < previous.size()){
                current[descents] += previous[descents]
                    * Modint<MOD>(
                        static_cast<long long>(descents + 1)
                    );
            }
            if(descents != 0){
                current[descents] += previous[descents - 1]
                    * Modint<MOD>(
                        static_cast<long long>(size - descents)
                    );
            }
        }
        previous = std::move(current);
    }
    return previous;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_EULERIAN_NUMBERS_HPP_INCLUDED
