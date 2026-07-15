#pragma once

#include <cstddef>

#include <stdexcept>
#include "geometry3d_sign.hpp"
#include "polygon3.hpp"
#include "signed_distance.hpp"

inline Polygon3 convex_polygon3_cut(
    const Polygon3& polygon,
    const Plane3& halfspace
){
    Polygon3 result;
    if(polygon.size() < 3)[[unlikely]]{
        throw std::invalid_argument("a polygon needs at least three vertices");
    }
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point3 current = polygon[index];
        const Point3 next = polygon[(index + 1) % polygon.size()];
        const long double first = signed_distance(halfspace, current);
        const long double second = signed_distance(halfspace, next);
        const int first_sign = geometry3d_sign(first);
        const int second_sign = geometry3d_sign(second);
        if(first_sign >= 0) result.push_back(current);
        if(first_sign * second_sign < 0){
            result.push_back(current + (next - current) * (first / (first - second)));
        }
    }
    return result;
}
