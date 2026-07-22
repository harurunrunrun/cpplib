#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_EPA_RESULT3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_EPA_RESULT3_HPP_INCLUDED

#include <cstddef>

#include "../core/geometry_primitives.hpp"

struct EPAResult3{
    bool intersects = false;
    long double penetration_depth = 0.0L;
    Point3 normal{};
    Point3 point_on_first{};
    Point3 point_on_second{};
    std::size_t iterations = 0;
    bool converged = false;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_EPA_RESULT3_HPP_INCLUDED
