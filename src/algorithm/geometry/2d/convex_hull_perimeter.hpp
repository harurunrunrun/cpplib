#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "convex_hull.hpp"
#include "distance.hpp"

inline long double convex_hull_perimeter(std::vector<Point> points){
    const std::vector<Point> hull = convex_hull(std::move(points));
    if(hull.size() <= 1) return 0.0L;
    if(hull.size() == 2) return 2.0L * distance(hull[0], hull[1]);
    long double result = 0.0L;
    for(std::size_t index = 0; index < hull.size(); ++index){
        result += distance(hull[index], hull[(index + 1) % hull.size()]);
    }
    return result;
}
