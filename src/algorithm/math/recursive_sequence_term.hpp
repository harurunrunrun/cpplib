#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

namespace recursive_sequence_internal{

inline std::vector<std::uint64_t> combine(
    const std::vector<std::uint64_t>& lhs,
    const std::vector<std::uint64_t>& rhs,
    const std::vector<std::uint64_t>& coefficient,
    std::uint64_t modulus
){
    const std::size_t degree = coefficient.size();
    std::vector<std::uint64_t> product(2 * degree - 1);
    for(std::size_t i = 0; i < degree; ++i){
        for(std::size_t j = 0; j < degree; ++j){
            product[i + j] = static_cast<std::uint64_t>(
                (product[i + j] + static_cast<unsigned __int128>(lhs[i]) * rhs[j]) %
                modulus
            );
        }
    }
    for(std::size_t degree_index = 2 * degree - 1;
        degree_index-- > degree;
    ){
        const std::uint64_t value = product[degree_index];
        for(std::size_t offset = 1; offset <= degree; ++offset){
            auto& target = product[degree_index - offset];
            target = static_cast<std::uint64_t>(
                (target + static_cast<unsigned __int128>(value) *
                    coefficient[offset - 1]) % modulus
            );
        }
    }
    product.resize(degree);
    return product;
}

} // namespace recursive_sequence_internal

// initial[0] is a_1; coefficient[0] multiplies a_(i-1).
inline std::uint64_t recursive_sequence_term(
    const std::vector<std::uint64_t>& initial,
    const std::vector<std::uint64_t>& coefficient,
    std::uint64_t one_based_index,
    std::uint64_t modulus = 1'000'000'000ULL
){
    if(initial.empty() || initial.size() != coefficient.size() ||
        one_based_index == 0 || modulus == 0
    )[[unlikely]]{
        throw std::invalid_argument("invalid recursive_sequence_term arguments");
    }
    const std::size_t degree = initial.size();
    if(one_based_index <= degree){
        return initial[static_cast<std::size_t>(one_based_index - 1)] % modulus;
    }
    std::vector<std::uint64_t> reduced_coefficient = coefficient;
    for(auto& value: reduced_coefficient) value %= modulus;
    std::vector<std::uint64_t> result(degree);
    std::vector<std::uint64_t> power(degree);
    result[0] = 1 % modulus;
    if(degree == 1){
        power[0] = reduced_coefficient[0];
    }else{
        power[1] = 1;
    }
    std::uint64_t exponent = one_based_index - 1;
    while(exponent != 0){
        if(exponent & 1){
            result = recursive_sequence_internal::combine(
                result, power, reduced_coefficient, modulus
            );
        }
        exponent >>= 1;
        if(exponent != 0){
            power = recursive_sequence_internal::combine(
                power, power, reduced_coefficient, modulus
            );
        }
    }
    std::uint64_t answer = 0;
    for(std::size_t i = 0; i < degree; ++i){
        answer = static_cast<std::uint64_t>(
            (answer + static_cast<unsigned __int128>(result[i]) *
                (initial[i] % modulus)) % modulus
        );
    }
    return answer;
}

} // namespace math
