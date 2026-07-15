#pragma once

#include "abs.hpp"
#include "base.hpp"
#include "ray_ray_closest_points.hpp"

inline long double distance(const Ray3& first, const Ray3& second){
    const auto points = closest_points(first, second);
    return abs(points.first - points.second);
}
