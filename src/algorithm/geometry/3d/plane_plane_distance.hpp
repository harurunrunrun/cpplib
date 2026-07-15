#pragma once

#include "abs.hpp"
#include "base.hpp"
#include "plane_plane_closest_points.hpp"

inline long double distance(const Plane3& first, const Plane3& second){
    const auto points = closest_points(first, second);
    return abs(points.first - points.second);
}
