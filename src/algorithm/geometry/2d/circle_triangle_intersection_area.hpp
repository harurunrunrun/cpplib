#pragma once

#include <vector>

#include "circle_polygon_intersection_area.hpp"

inline long double circle_triangle_intersection_area(
    const Circle& circle,
    const Point& first,
    const Point& second,
    const Point& third
){
    return circle_polygon_intersection_area(
        circle, std::vector<Point>{first, second, third}
    );
}
