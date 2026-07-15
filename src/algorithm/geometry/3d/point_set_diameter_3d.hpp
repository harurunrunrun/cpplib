#pragma once

#include <cmath>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"

struct PointSetDiameter3DResult{
    std::size_t first;
    std::size_t second;
    long double distance;
};

inline std::optional<PointSetDiameter3DResult> point_set_diameter_3d(
    const std::vector<Point3>& points
){
    for(const Point3& point: points){
        if(!std::isfinite(point.x) || !std::isfinite(point.y) ||
           !std::isfinite(point.z))[[unlikely]]{
            throw std::invalid_argument("point_set_diameter_3d requires finite points");
        }
    }
    if(points.size() < 2) return std::nullopt;
    PointSetDiameter3DResult result{
        0,
        1,
        std::hypot(
            points[0].x - points[1].x,
            points[0].y - points[1].y,
            points[0].z - points[1].z
        ),
    };
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1; second < points.size(); ++second){
            const long double current_distance = std::hypot(
                points[first].x - points[second].x,
                points[first].y - points[second].y,
                points[first].z - points[second].z
            );
            if(current_distance > result.distance ||
               (current_distance == result.distance &&
                std::pair{first, second} < std::pair{result.first, result.second})){
                result = {first, second, current_distance};
            }
        }
    }
    return result;
}
