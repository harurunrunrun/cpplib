#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CONVOLUTION_MOD_2_64_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CONVOLUTION_MOD_2_64_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "convolution.hpp"

inline constexpr std::size_t convolution_mod_2_64_max_input_size = 524288;

namespace convolution_mod_2_64_detail{

inline constexpr std::array<std::uint64_t, 5> primes{
    998244353ULL,
    1004535809ULL,
    985661441ULL,
    943718401ULL,
    935329793ULL,
};

inline std::uint64_t power_mod(
    std::uint64_t base,
    std::uint64_t exponent,
    std::uint64_t modulus
){
    std::uint64_t result = 1;
    while(exponent > 0){
        if(exponent & 1U){
            result = static_cast<std::uint64_t>(
                static_cast<unsigned __int128>(result) * base % modulus
            );
        }
        base = static_cast<std::uint64_t>(
            static_cast<unsigned __int128>(base) * base % modulus
        );
        exponent >>= 1;
    }
    return result;
}

template<int Modulus>
std::vector<std::uint32_t> residue_convolution(
    const std::vector<std::uint64_t>& first,
    const std::vector<std::uint64_t>& second
){
    using Mint = Modint<Modulus>;
    std::vector<Mint> left(first.size()), right(second.size());
    for(std::size_t index = 0; index < first.size(); ++index){
        left[index] = Mint{
            static_cast<long long>(
                first[index] % static_cast<std::uint64_t>(Modulus)
            )
        };
    }
    for(std::size_t index = 0; index < second.size(); ++index){
        right[index] = Mint{
            static_cast<long long>(
                second[index] % static_cast<std::uint64_t>(Modulus)
            )
        };
    }
    const std::vector<Mint> product =
        convolution_ntt(std::move(left), std::move(right));
    std::vector<std::uint32_t> result(product.size());
    for(std::size_t index = 0; index < product.size(); ++index){
        result[index] = static_cast<std::uint32_t>(product[index].val());
    }
    return result;
}

inline std::array<std::uint64_t, 5> mixed_radix_inverses(){
    std::array<std::uint64_t, 5> result{};
    result[0] = 1;
    for(std::size_t index = 1; index < primes.size(); ++index){
        std::uint64_t product = 1;
        for(std::size_t preceding = 0; preceding < index; ++preceding){
            product = static_cast<std::uint64_t>(
                static_cast<unsigned __int128>(product)
                * primes[preceding] % primes[index]
            );
        }
        result[index] = power_mod(
            product,
            primes[index] - 2,
            primes[index]
        );
    }
    return result;
}

} // namespace convolution_mod_2_64_detail

inline std::vector<std::uint64_t> convolution_mod_2_64(
    const std::vector<std::uint64_t>& first,
    const std::vector<std::uint64_t>& second
){
    using namespace convolution_mod_2_64_detail;
    if(first.empty() || second.empty()) return {};
    if(first.size() > convolution_mod_2_64_max_input_size
        || second.size() > convolution_mod_2_64_max_input_size)[[unlikely]]{
        throw std::length_error(
            "convolution_mod_2_64 input exceeds its exact CRT bound"
        );
    }

    const std::array<std::vector<std::uint32_t>, 5> residues{
        residue_convolution<998244353>(first, second),
        residue_convolution<1004535809>(first, second),
        residue_convolution<985661441>(first, second),
        residue_convolution<943718401>(first, second),
        residue_convolution<935329793>(first, second),
    };
    const std::array<std::uint64_t, 5> inverse =
        mixed_radix_inverses();
    const std::size_t result_size = residues[0].size();
    std::vector<std::uint64_t> result(result_size);
    std::array<std::uint64_t, 5> digit{};

    for(std::size_t coefficient = 0;
        coefficient < result_size;
        ++coefficient){
        std::uint64_t reconstructed = 0;
        std::uint64_t product_mod_2_64 = 1;
        for(std::size_t index = 0; index < primes.size(); ++index){
            const std::uint64_t modulus = primes[index];
            std::uint64_t partial = 0;
            std::uint64_t radix_product = 1;
            for(std::size_t preceding = 0;
                preceding < index;
                ++preceding){
                partial = static_cast<std::uint64_t>(
                    (
                        partial
                        + static_cast<unsigned __int128>(
                            radix_product
                        ) * digit[preceding]
                    ) % modulus
                );
                radix_product = static_cast<std::uint64_t>(
                    static_cast<unsigned __int128>(radix_product)
                    * primes[preceding] % modulus
                );
            }
            const std::uint64_t residue = residues[index][coefficient];
            const std::uint64_t difference =
                residue >= partial
                    ? residue - partial
                    : residue + modulus - partial;
            digit[index] = static_cast<std::uint64_t>(
                static_cast<unsigned __int128>(difference)
                * inverse[index] % modulus
            );
            reconstructed += product_mod_2_64 * digit[index];
            product_mod_2_64 *= modulus;
        }
        result[coefficient] = reconstructed;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CONVOLUTION_MOD_2_64_HPP_INCLUDED
