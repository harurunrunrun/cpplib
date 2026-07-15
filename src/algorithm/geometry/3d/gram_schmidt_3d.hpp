#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "dot.hpp"

inline std::vector<Point3> gram_schmidt_3d(
    const std::vector<Point3>& vectors,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    if(relative_epsilon < 0 || !std::isfinite(relative_epsilon)){
        throw std::invalid_argument("relative epsilon must be finite and nonnegative");
    }
    std::vector<Point3> result;
    result.reserve(3);
    for(const Point3& input: vectors){
        if(!std::isfinite(input.x) || !std::isfinite(input.y) ||
           !std::isfinite(input.z)){
            throw std::invalid_argument("vectors must be finite");
        }
        const long double scale = std::max({
            std::abs(input.x), std::abs(input.y), std::abs(input.z)
        });
        if(scale == 0) continue;
        Point3 value = input / scale;
        for(int pass = 0; pass < 2; ++pass){
            for(const Point3& basis: result) value -= basis * dot(value, basis);
        }
        const long double length = std::hypot(value.x, value.y, value.z);
        if(length <= relative_epsilon) continue;
        result.push_back(value / length);
        if(result.size() == 3) break;
    }
    return result;
}
