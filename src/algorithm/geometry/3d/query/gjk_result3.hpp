#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_GJK_RESULT3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_GJK_RESULT3_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "../core/geometry_primitives.hpp"

struct GJKSupportPoint3{
    Point3 point{};
    Point3 point_on_first{};
    Point3 point_on_second{};
};

struct GJKResult3{
    bool intersects = false;
    long double distance = 0.0L;
    Point3 point_on_first{};
    Point3 point_on_second{};
    Point3 normal{};
    std::vector<GJKSupportPoint3> simplex;
    std::size_t iterations = 0;
    bool converged = false;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_GJK_RESULT3_HPP_INCLUDED
