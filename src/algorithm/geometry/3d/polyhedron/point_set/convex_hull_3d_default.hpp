#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POINT_SET_CONVEX_HULL_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POINT_SET_CONVEX_HULL_3D_DEFAULT_HPP_INCLUDED

#include <vector>

#include "divide_and_conquer_convex_hull_3d.hpp"

inline ConvexPolyhedron3 convex_hull_3d(std::vector<Point3> input){
    return divide_and_conquer_convex_hull_3d(std::move(input));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POINT_SET_CONVEX_HULL_3D_DEFAULT_HPP_INCLUDED
