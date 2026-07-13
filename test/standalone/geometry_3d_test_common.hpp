#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/geometry_3d/base.hpp"

inline bool geometry3d_close_value(
    long double left,
    long double right,
    long double epsilon = 1e-9L
){
    return std::abs(left - right) <= epsilon *
        std::max({1.0L, std::abs(left), std::abs(right)});
}

inline bool geometry3d_close_point(
    const Point3& left,
    const Point3& right,
    long double epsilon = 1e-9L
){
    return geometry3d_close_value(left.x, right.x, epsilon) &&
        geometry3d_close_value(left.y, right.y, epsilon) &&
        geometry3d_close_value(left.z, right.z, epsilon);
}

template<class Function>
int geometry3d_test_main(Function function){
    int repeat;
    if(!(std::cin >> repeat)) return 0;
    for(int iteration = 0; iteration < repeat; iteration++){
        if(!function()) return 1;
    }
    std::cout << "OK\n";
    return 0;
}
