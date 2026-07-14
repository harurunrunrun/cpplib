#pragma once

#include <cstdint>

#include "counting_squarefrees.hpp"

namespace math{

// Returns the number of positive square-free integers not exceeding n.
inline std::uint64_t squarefree_integer_count(std::uint64_t n){
    return count_squarefree_numbers(n);
}

} // namespace math
