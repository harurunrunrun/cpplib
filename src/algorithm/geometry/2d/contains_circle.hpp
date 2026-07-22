#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONTAINS_CIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONTAINS_CIRCLE_HPP_INCLUDED

#include "distance.hpp"
#include "validate_circle.hpp"

inline bool contains_circle(const Circle& circle, const Point& point){
    validate_circle(circle);
    return geometry_sign(distance(circle.center, point) - circle.radius) <= 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONTAINS_CIRCLE_HPP_INCLUDED
