#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_RAY_LINE_RAY_OVERLAP_LINE_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_RAY_LINE_RAY_OVERLAP_LINE_RAY_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../result/line_ray/line_ray_intersection.hpp"

inline bool overlap(const Line3& first, const Ray3& second){
    return std::holds_alternative<Ray3>(line_ray_intersection(first, second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_RAY_LINE_RAY_OVERLAP_LINE_RAY_HPP_INCLUDED
