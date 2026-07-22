#ifndef CPPLIB_SRC_ALGORITHM_MATH_INTERSECTION_OF_F2_VECTOR_SPACES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_INTERSECTION_OF_F2_VECTOR_SPACES_HPP_INCLUDED

#include <array>
#include <concepts>
#include <cstddef>
#include <limits>
#include <vector>

template<std::unsigned_integral UInt>
std::vector<UInt> intersection_of_f2_vector_spaces(
    const std::vector<UInt>& first,
    const std::vector<UInt>& second
){
    constexpr std::size_t bit_width = std::numeric_limits<UInt>::digits;
    using Basis = std::array<UInt, bit_width>;

    const auto insert = [](Basis& basis, UInt value){
        for(std::size_t offset = bit_width; offset > 0; --offset){
            const std::size_t bit = offset - 1;
            if(((value >> bit) & UInt(1)) == UInt(0)) continue;
            if(basis[bit] != UInt(0)){
                value ^= basis[bit];
            }else{
                basis[bit] = value;
                return;
            }
        }
    };

    Basis first_basis{};
    for(UInt value: first) insert(first_basis, value);

    Basis second_basis{};
    for(UInt value: second) insert(second_basis, value);

    Basis quotient_basis{};
    Basis quotient_first_part{};
    Basis intersection_basis{};
    for(std::size_t offset = bit_width; offset > 0; --offset){
        const UInt second_vector = second_basis[offset - 1];
        if(second_vector == UInt(0)) continue;

        UInt residual = second_vector;
        UInt first_part = UInt(0);
        for(std::size_t first_offset = bit_width; first_offset > 0; --first_offset){
            const std::size_t bit = first_offset - 1;
            if(((residual >> bit) & UInt(1)) != UInt(0) &&
               first_basis[bit] != UInt(0)){
                residual ^= first_basis[bit];
                first_part ^= first_basis[bit];
            }
        }

        bool inserted = false;
        for(std::size_t quotient_offset = bit_width;
            quotient_offset > 0;
            --quotient_offset
        ){
            const std::size_t bit = quotient_offset - 1;
            if(((residual >> bit) & UInt(1)) == UInt(0)) continue;
            if(quotient_basis[bit] != UInt(0)){
                residual ^= quotient_basis[bit];
                first_part ^= quotient_first_part[bit];
            }else{
                quotient_basis[bit] = residual;
                quotient_first_part[bit] = first_part;
                inserted = true;
                break;
            }
        }
        if(!inserted) insert(intersection_basis, first_part);
    }

    for(std::size_t low = 0; low < bit_width; ++low){
        if(intersection_basis[low] == UInt(0)) continue;
        for(std::size_t high = low + 1; high < bit_width; ++high){
            if(((intersection_basis[high] >> low) & UInt(1)) != UInt(0)){
                intersection_basis[high] ^= intersection_basis[low];
            }
        }
    }

    std::vector<UInt> result;
    result.reserve(bit_width);
    for(std::size_t offset = bit_width; offset > 0; --offset){
        if(intersection_basis[offset - 1] != UInt(0)){
            result.push_back(intersection_basis[offset - 1]);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_INTERSECTION_OF_F2_VECTOR_SPACES_HPP_INCLUDED
