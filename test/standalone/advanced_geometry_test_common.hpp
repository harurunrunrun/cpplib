#pragma once

#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

inline constexpr long double ADVANCED_GEOMETRY_OUTPUT_SCALE = 1000000.0L;

inline std::vector<Point> advanced_geometry_read_polygon(){
    int count;
    std::cin >> count;
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    return polygon;
}

inline void advanced_geometry_print_polygon(const std::vector<Point>& polygon){
    std::cout << polygon.size();
    for(const Point& point: polygon){
        std::cout << ' ' << std::llround(point.x * ADVANCED_GEOMETRY_OUTPUT_SCALE)
                  << ' ' << std::llround(point.y * ADVANCED_GEOMETRY_OUTPUT_SCALE);
    }
    std::cout << '\n';
}
