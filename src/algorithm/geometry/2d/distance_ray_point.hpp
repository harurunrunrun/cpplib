#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DISTANCE_RAY_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DISTANCE_RAY_POINT_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "ray.hpp"

inline long double distance(const Ray& ray, const Point& point){
    const Point direction = ray.through - ray.origin;
    const long double length = abs(direction);
    if(geometry_sign(length) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate ray");
    }
    const Point unit_direction = direction / length;
    if(geometry_sign(dot(point - ray.origin, unit_direction)) <= 0){
        return abs(point - ray.origin);
    }
    return std::abs(cross(unit_direction, point - ray.origin));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DISTANCE_RAY_POINT_HPP_INCLUDED
