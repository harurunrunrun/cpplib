#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_ON_CIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_ON_CIRCLE_HPP_INCLUDED

#include "../scalar/distance.hpp"
#include "validate_circle.hpp"

inline bool on_circle(const Circle& circle, const Point& point){
    validate_circle(circle);
    return geometry_sign(distance(circle.center, point) - circle.radius) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_ON_CIRCLE_HPP_INCLUDED
