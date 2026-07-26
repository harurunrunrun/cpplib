#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_ARRANGEMENT_RESULT3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_ARRANGEMENT_RESULT3_HPP_INCLUDED

#include <cstdint>
#include <vector>

#include "geometry_primitives.hpp"

struct PlaneArrangement3{
    std::vector<Plane3> planes;
    std::vector<Line3> intersection_lines;
    std::vector<Point3> intersection_points;
    std::uint64_t region_count = 1;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_ARRANGEMENT_RESULT3_HPP_INCLUDED
