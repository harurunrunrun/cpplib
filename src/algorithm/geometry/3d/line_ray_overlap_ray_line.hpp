#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_OVERLAP_RAY_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_OVERLAP_RAY_LINE_HPP_INCLUDED

#include "line_ray_overlap_line_ray.hpp"

inline bool overlap(const Ray3& first, const Line3& second){
    return overlap(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_OVERLAP_RAY_LINE_HPP_INCLUDED