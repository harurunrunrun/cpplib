#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CLOSEST_PAIR_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CLOSEST_PAIR_RESULT_HPP_INCLUDED

#include "../../math/integer/exact_integer.hpp"

#include <cstddef>
#include <limits>

struct ClosestPairResult{
    std::size_t first = std::numeric_limits<std::size_t>::max();
    std::size_t second = std::numeric_limits<std::size_t>::max();
    ExactInteger squared_distance = 0;

    bool exists() const{
        return first != std::numeric_limits<std::size_t>::max();
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CLOSEST_PAIR_RESULT_HPP_INCLUDED
