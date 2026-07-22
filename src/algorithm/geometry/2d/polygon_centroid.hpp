#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_CENTROID_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_CENTROID_HPP_INCLUDED

#include <stdexcept>
#include <vector>

#include "advanced/detail.hpp"

inline Point polygon_centroid(const std::vector<Point>& polygon){
    if(polygon.size() < 3)[[unlikely]]{
        throw std::invalid_argument("polygon has zero signed area");
    }
    const Point& origin = polygon[0];
    long double doubled_area = 0.0L;
    long double area_scale = 0.0L;
    Point local_weighted{};
    for(std::size_t index = 1; index + 1 < polygon.size(); ++index){
        const Point first = polygon[index] - origin;
        const Point second = polygon[index + 1] - origin;
        const long double determinant = cross(first, second);
        doubled_area += determinant;
        area_scale += advanced_geometry_detail::length(first) *
            advanced_geometry_detail::length(second);
        local_weighted += (first + second) * determinant;
    }
    if(advanced_geometry_detail::scaled_sign(
        doubled_area, area_scale
    ) == 0)[[unlikely]]{
        throw std::invalid_argument("polygon has zero signed area");
    }
    return origin +
        local_weighted / (3.0L * doubled_area);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_CENTROID_HPP_INCLUDED
