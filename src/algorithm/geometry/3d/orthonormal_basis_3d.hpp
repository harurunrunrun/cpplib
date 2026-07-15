#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "cross.hpp"

inline std::array<Point3, 3> orthonormal_basis_3d(const Point3& first_axis){
    if(!std::isfinite(first_axis.x) || !std::isfinite(first_axis.y) ||
       !std::isfinite(first_axis.z)){
        throw std::invalid_argument("first axis must be finite");
    }
    const long double scale = std::max({
        std::abs(first_axis.x), std::abs(first_axis.y), std::abs(first_axis.z)
    });
    if(scale == 0) throw std::invalid_argument("first axis must be nonzero");
    const Point3 scaled = first_axis / scale;
    const long double length = std::hypot(scaled.x, scaled.y, scaled.z);
    const Point3 first = scaled / length;
    Point3 helper;
    if(std::abs(first.x) <= std::abs(first.y) &&
       std::abs(first.x) <= std::abs(first.z)){
        helper = {1, 0, 0};
    }else if(std::abs(first.y) <= std::abs(first.z)){
        helper = {0, 1, 0};
    }else{
        helper = {0, 0, 1};
    }
    Point3 second = cross(first, helper);
    second /= std::hypot(second.x, second.y, second.z);
    const Point3 third = cross(first, second);
    return {first, second, third};
}
