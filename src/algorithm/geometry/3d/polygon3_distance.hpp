#pragma once

#include <algorithm>
#include <limits>

#include "distance.hpp"
#include "polygon3_contains.hpp"
#include "polygon3_normal.hpp"
#include "projection.hpp"

inline long double polygon3_distance(
    const Polygon3& polygon,
    const Point3& point
){
    const Plane3 plane{polygon.front(), polygon3_normal(polygon)};
    const Point3 projected = projection(plane, point);
    if(polygon3_contains(polygon, projected) != 0){
        return distance(plane, point);
    }
    long double result = std::numeric_limits<long double>::infinity();
    for(std::size_t index = 0; index < polygon.size(); ++index){
        result = std::min(result, distance(
            Segment3{polygon[index], polygon[(index + 1) % polygon.size()]},
            point
        ));
    }
    return result;
}
