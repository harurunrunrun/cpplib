#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_VIEW_CONE_CONTAINS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_VIEW_CONE_CONTAINS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../core/constants.hpp"
#include "../scalar/cross.hpp"
#include "../scalar/dot.hpp"
#include "../scalar/norm.hpp"
#include "../core/types.hpp"

inline bool view_cone_contains(
    const Point& observer,
    const Point& direction,
    long double aperture,
    const Point& point,
    bool include_boundary = true
){
    const long double turn = 2.0L * std::acos(-1.0L);
    const auto finite_point = [](const Point& value){
        return std::isfinite(value.x) && std::isfinite(value.y);
    };
    if(!finite_point(observer) || !finite_point(direction)
        || !finite_point(point) || !std::isfinite(aperture))[[unlikely]]{
        throw std::invalid_argument("view cone input must be finite");
    }
    if(aperture < -GEOMETRY_EPS
        || aperture > turn + GEOMETRY_EPS)[[unlikely]]{
        throw std::invalid_argument("view cone aperture is outside [0, 2*pi]");
    }
    if(norm(direction) <= GEOMETRY_EPS * GEOMETRY_EPS)[[unlikely]]{
        throw std::invalid_argument("view cone direction must be nonzero");
    }
    const Point relative = point - observer;
    if(norm(relative) <= GEOMETRY_EPS * GEOMETRY_EPS) return true;
    aperture = std::clamp(aperture, 0.0L, turn);
    if(aperture >= turn - GEOMETRY_EPS) return true;
    const long double angle = std::atan2(
        std::fabs(cross(direction, relative)),
        dot(direction, relative)
    );
    const long double half = aperture / 2.0L;
    if(include_boundary) return angle <= half + GEOMETRY_EPS;
    return angle < half - GEOMETRY_EPS;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_VIEW_CONE_CONTAINS_HPP_INCLUDED
