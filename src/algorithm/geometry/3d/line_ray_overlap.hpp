#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_OVERLAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_OVERLAP_HPP_INCLUDED

#include <variant>

#include "geometry_primitives.hpp"
#include "line_ray_intersection.hpp"

inline bool overlap(const Line3& first, const Ray3& second){
    return std::holds_alternative<Ray3>(line_ray_intersection(first, second));
}

inline bool overlap(const Ray3& first, const Line3& second){
    return overlap(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_OVERLAP_HPP_INCLUDED
