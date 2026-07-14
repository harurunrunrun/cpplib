#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "advanced/convex_polygon_minkowski_difference.hpp"
#include "convex_polygon_query.hpp"
#include "distance.hpp"

inline long double convex_polygon_minimum_distance(
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
    const Point origin{};
    if(difference.size() == 1) return distance(difference[0], origin);
    if(difference.size() == 2){
        return distance_segment_point(
            {difference[0], difference[1]}, origin
        );
    }
    if(ConvexPolygonQuery(difference).contains(origin) != 0) return 0.0L;

    long double result = std::numeric_limits<long double>::infinity();
    for(std::size_t index = 0; index < difference.size(); ++index){
        result = std::min(result, distance_segment_point(
            {difference[index], difference[(index + 1) % difference.size()]},
            origin
        ));
    }
    return result;
}
