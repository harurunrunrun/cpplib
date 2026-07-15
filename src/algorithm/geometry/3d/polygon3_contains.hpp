#pragma once

#include "../2d/contains.hpp"
#include <algorithm>
#include <cmath>

#include "abs.hpp"
#include "dot.hpp"
#include "polygon3_to_2d.hpp"

inline int polygon3_contains(const Polygon3& polygon, const Point3& point){
    const auto [system, projected] = polygon3_to_2d(polygon);
    const long double height = dot(point - system.origin, system.normal);
    if(std::abs(height) > GEOMETRY3D_EPS *
        std::max(1.0L, abs(point - system.origin))){
        return 0;
    }
    return contains(projected, to_plane_coordinates(system, point));
}
