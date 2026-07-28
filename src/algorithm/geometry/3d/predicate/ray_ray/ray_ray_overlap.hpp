#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_RAY_RAY_RAY_OVERLAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_RAY_RAY_RAY_OVERLAP_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../result/ray_ray/ray_ray_intersection.hpp"

inline bool overlap(const Ray3& first, const Ray3& second){
    const LinearIntersection3 intersection = ray_ray_intersection(first, second);
    return std::holds_alternative<Ray3>(intersection) ||
        std::holds_alternative<Segment3>(intersection);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_RAY_RAY_RAY_OVERLAP_HPP_INCLUDED
