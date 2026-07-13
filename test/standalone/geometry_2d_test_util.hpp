#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/types.hpp"

inline void print_geometry_scalar(long double value){
    if(std::abs(value) < 0.5e-12L) value = 0.0L;
    std::cout << std::fixed << std::setprecision(9) << value;
}

inline void print_geometry_point(const Point& point){
    print_geometry_scalar(point.x);
    std::cout << ' ';
    print_geometry_scalar(point.y);
}

inline void print_geometry_points(const std::vector<Point>& points){
    std::cout << points.size();
    for(const Point& point: points){
        std::cout << ' ';
        print_geometry_point(point);
    }
    std::cout << '\n';
}
