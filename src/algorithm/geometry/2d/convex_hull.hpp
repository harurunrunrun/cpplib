#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "cross.hpp"

inline std::vector<Point> convex_hull(std::vector<Point> points){
    std::sort(points.begin(), points.end());
    points.erase(std::unique(
        points.begin(), points.end(),
        [](const Point& left, const Point& right){
            return left.x == right.x && left.y == right.y;
        }
    ), points.end());
    const int size = static_cast<int>(points.size());
    if(size <= 1) return points;
    std::vector<Point> hull(static_cast<std::size_t>(2 * size));
    int count = 0;
    for(int index = 0; index < size; ++index){
        while(count >= 2 && geometry_sign(cross(
            hull[static_cast<std::size_t>(count - 1)] -
                hull[static_cast<std::size_t>(count - 2)],
            points[static_cast<std::size_t>(index)] -
                hull[static_cast<std::size_t>(count - 1)]
        )) <= 0){
            --count;
        }
        hull[static_cast<std::size_t>(count++)] =
            points[static_cast<std::size_t>(index)];
    }
    for(int index = size - 2, lower_end = count + 1; index >= 0; --index){
        while(count >= lower_end && geometry_sign(cross(
            hull[static_cast<std::size_t>(count - 1)] -
                hull[static_cast<std::size_t>(count - 2)],
            points[static_cast<std::size_t>(index)] -
                hull[static_cast<std::size_t>(count - 1)]
        )) <= 0){
            --count;
        }
        hull[static_cast<std::size_t>(count++)] =
            points[static_cast<std::size_t>(index)];
    }
    hull.resize(static_cast<std::size_t>(count - 1));
    return hull;
}
