#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_COUNTERCLOCKWISE_POLYGON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_COUNTERCLOCKWISE_POLYGON_HPP_INCLUDED

#include <algorithm>
#include <vector>

#include "../detail/advanced_convex_geometry_detail.hpp"

inline std::vector<Point> counterclockwise_polygon(std::vector<Point> polygon){
    long double doubled_area = 0.0L;
    long double area_scale = 0.0L;
    if(polygon.size() >= 3){
        const Point& origin = polygon[0];
        for(std::size_t index = 1; index + 1 < polygon.size(); ++index){
            const Point first = polygon[index] - origin;
            const Point second = polygon[index + 1] - origin;
            doubled_area += cross(first, second);
            area_scale += advanced_geometry_detail::length(first) *
                advanced_geometry_detail::length(second);
        }
    }
    if(advanced_geometry_detail::scaled_sign(
        doubled_area, area_scale
    ) < 0){
        std::reverse(polygon.begin() + 1, polygon.end());
    }
    return polygon;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_COUNTERCLOCKWISE_POLYGON_HPP_INCLUDED
