#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

#include "advanced/convex_polygon_minkowski_difference.hpp"
#include "norm.hpp"

inline long double convex_polygon_maximum_distance(
    std::vector<Point> first,
    std::vector<Point> second
){
    const std::vector<Point> difference =
        convex_polygon_minkowski_difference(
            std::move(first), std::move(second)
        );
    if(difference.empty()){
        throw std::invalid_argument("distance to an empty convex polygon");
    }
    long double squared_result = 0.0L;
    for(const Point& point: difference){
        squared_result = std::max(squared_result, norm(point));
    }
    return std::sqrt(squared_result);
}
