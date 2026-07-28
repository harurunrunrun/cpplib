#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_SET_POINT_SET_CONVEX_HULL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_SET_POINT_SET_CONVEX_HULL_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../integer/point_point_point/orientation.hpp"

namespace integer_geometry{

inline std::vector<Point> convex_hull(std::vector<Point> points){
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    if(points.size() <= 1) return points;
    if(points.size() > std::numeric_limits<std::size_t>::max() / 2){
        throw std::length_error("integer geometry convex hull is too large");
    }
    std::vector<Point> hull(points.size() * 2);
    std::size_t size = 0;
    for(const Point& point: points){
        while(size >= 2
            && orientation(hull[size - 2], hull[size - 1], point) <= 0){
            --size;
        }
        hull[size++] = point;
    }
    const std::size_t lower_size = size;
    for(std::size_t index = points.size() - 1; index-- > 0;){
        const Point& point = points[index];
        while(size > lower_size
            && orientation(hull[size - 2], hull[size - 1], point) <= 0){
            --size;
        }
        hull[size++] = point;
    }
    hull.resize(size - 1);
    return hull;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_SET_POINT_SET_CONVEX_HULL_HPP_INCLUDED
