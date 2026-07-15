#pragma once

#include <cmath>
#include <vector>

#include "spherical_central_angle.hpp"

inline bool points_in_hemisphere(const std::vector<Point3>& points){
    if(points.empty()) return true;
    std::vector<Point3> unit;
    unit.reserve(points.size());
    for(const Point3& point: points){
        unit.push_back(geometry3d_spherical_detail::unit_direction(
            point, "invalid point in hemisphere containment"
        ));
    }
    const long double tolerance = 256.0L
        * std::numeric_limits<long double>::epsilon();
    const auto accepts = [&](const Point3& normal){
        for(const Point3& point: unit){
            if(dot(normal, point) < -tolerance) return false;
        }
        return true;
    };
    for(const Point3& point: unit) if(accepts(point)) return true;
    for(std::size_t first = 0; first < unit.size(); ++first){
        for(std::size_t second = first + 1; second < unit.size(); ++second){
            const Point3 product = cross(unit[first], unit[second]);
            const long double length = std::hypot(product.x, product.y, product.z);
            if(length <= tolerance) continue;
            const Point3 normal = product / length;
            if(accepts(normal) || accepts(-normal)) return true;
        }
    }
    bool all_parallel = true;
    for(std::size_t index = 1; index < unit.size(); ++index){
        const Point3 product = cross(unit[0], unit[index]);
        if(std::hypot(product.x, product.y, product.z) > tolerance){
            all_parallel = false;
            break;
        }
    }
    return all_parallel;
}
