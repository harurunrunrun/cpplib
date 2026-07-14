#pragma once

#include <cmath>
#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "ray.hpp"

inline long double distance(const Ray& ray, const Point& point){
    const Point direction = ray.through - ray.origin;
    if(geometry_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate ray");
    }
    if(geometry_sign(dot(point - ray.origin, direction)) <= 0){
        return abs(point - ray.origin);
    }
    return std::abs(cross(direction, point - ray.origin)) / abs(direction);
}
