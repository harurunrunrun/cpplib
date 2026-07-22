#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_SQUAREFREE_INTEGER_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_SQUAREFREE_INTEGER_COUNT_HPP_INCLUDED

#include <cstdint>

#include "counting_squarefrees.hpp"

namespace math{

// Returns the number of positive square-free integers not exceeding n.
inline std::uint64_t squarefree_integer_count(std::uint64_t n){
    return count_squarefree_numbers(n);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_SQUAREFREE_INTEGER_COUNT_HPP_INCLUDED
