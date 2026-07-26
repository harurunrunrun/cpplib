#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CLOSEST_PAIR_3D_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CLOSEST_PAIR_3D_RESULT_HPP_INCLUDED

#include <cstddef>

struct ClosestPair3DResult{
    std::size_t first;
    std::size_t second;
    long double distance;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CLOSEST_PAIR_3D_RESULT_HPP_INCLUDED
