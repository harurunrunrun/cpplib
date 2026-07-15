#pragma once

#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "is_finite.hpp"
#include "relative_equal.hpp"

inline std::vector<Point3> deduplicate_points_3d(
    const std::vector<Point3>& points,
    long double relative_tolerance = 0.0L,
    long double absolute_tolerance = GEOMETRY3D_EPS
){
    if(!geometry3d_is_finite(relative_tolerance)
        || !geometry3d_is_finite(absolute_tolerance)
        || relative_tolerance < 0 || absolute_tolerance < 0)[[unlikely]]{
        throw std::invalid_argument("invalid 3D point deduplication tolerance");
    }
    std::vector<Point3> result;
    result.reserve(points.size());
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite point in 3D point set");
        }
        bool duplicate = false;
        for(const Point3& representative: result){
            if(relative_equal(point.x, representative.x, relative_tolerance, absolute_tolerance)
                && relative_equal(point.y, representative.y, relative_tolerance, absolute_tolerance)
                && relative_equal(point.z, representative.z, relative_tolerance, absolute_tolerance)){
                duplicate = true;
                break;
            }
        }
        if(!duplicate) result.push_back(point);
    }
    return result;
}
