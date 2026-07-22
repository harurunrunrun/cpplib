#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_SAT_RESULT3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_SAT_RESULT3_HPP_INCLUDED

#include <cstddef>

#include "../core/geometry_primitives.hpp"

struct SATResult3{
    bool intersects = false;
    long double separation = 0.0L;
    long double penetration_depth = 0.0L;
    Point3 axis{};
    std::size_t tested_axes = 0;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_SAT_RESULT3_HPP_INCLUDED
