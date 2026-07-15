#pragma once

#include "abs.hpp"
#include "base.hpp"
#include "ray_plane_closest_points.hpp"

inline long double distance(const Ray3& ray, const Plane3& plane){
    const auto points = closest_points(ray, plane);
    return abs(points.first - points.second);
}

inline long double distance(const Plane3& plane, const Ray3& ray){
    return distance(ray, plane);
}
