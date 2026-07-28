#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_CATALAN_NUMBERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_CATALAN_NUMBERS_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../../../structure/modint/modint.hpp"

namespace math{

template<int MOD>
std::vector<Modint<MOD>> catalan_numbers(std::size_t maximum_index){
    if(maximum_index > static_cast<std::size_t>((MOD - 2) / 2))[[unlikely]]{
        throw std::length_error(
            "catalan_numbers requires 2 * maximum_index + 1 < MOD"
        );
    }
    std::vector<Modint<MOD>> result(maximum_index + 1);
    result[0] = Modint<MOD>(1);
    for(std::size_t index = 0; index < maximum_index; ++index){
        result[index + 1] = result[index]
            * Modint<MOD>(static_cast<long long>(4 * index + 2))
            / Modint<MOD>(static_cast<long long>(index + 2));
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_CATALAN_NUMBERS_HPP_INCLUDED
