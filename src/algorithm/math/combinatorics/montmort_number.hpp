#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_MONTMORT_NUMBER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_MONTMORT_NUMBER_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

inline std::vector<std::uint32_t> montmort_numbers(
    std::size_t maximum_size, std::uint32_t modulus
){
    if(modulus == 0){
        throw std::invalid_argument("montmort_numbers requires modulus > 0");
    }
    std::vector<std::uint32_t> result(maximum_size + 1);
    result[0] = 1 % modulus;
    if(maximum_size == 0) return result;
    result[1] = 0;
    for(std::size_t size = 2; size <= maximum_size; ++size){
        const std::uint64_t sum =
            (static_cast<std::uint64_t>(result[size - 1]) +
             result[size - 2]) % modulus;
        const std::uint64_t factor =
            static_cast<std::uint64_t>(size - 1) % modulus;
        result[size] = static_cast<std::uint32_t>(factor * sum % modulus);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_MONTMORT_NUMBER_HPP_INCLUDED
