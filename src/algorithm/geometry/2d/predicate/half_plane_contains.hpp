#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_HALF_PLANE_CONTAINS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_HALF_PLANE_CONTAINS_HPP_INCLUDED

#include <stdexcept>

#include "../detail/advanced_convex_geometry_detail.hpp"

inline bool half_plane_contains(const Line& half_plane, const Point& point){
    const Point direction = half_plane.b - half_plane.a;
    if(advanced_geometry_detail::length(direction) <= GEOMETRY_EPS){
        throw std::invalid_argument("a half-plane boundary requires two distinct points");
    }
    return advanced_geometry_detail::side_sign(half_plane, point) >= 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_HALF_PLANE_CONTAINS_HPP_INCLUDED
