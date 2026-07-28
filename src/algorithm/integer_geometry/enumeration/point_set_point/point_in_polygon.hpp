#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_ENUMERATION_POINT_SET_POINT_POINT_IN_POLYGON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_ENUMERATION_POINT_SET_POINT_POINT_IN_POLYGON_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "../../predicate/segment_point/on_segment.hpp"

namespace integer_geometry{

inline PointLocation point_in_polygon(
    const std::vector<Point>& polygon,
    const Point& point
){
    if(polygon.empty()) return PointLocation::outside;
    int winding_number = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % polygon.size()];
        if(on_segment({first, second}, point)) return PointLocation::boundary;
        if(first.y <= point.y && point.y < second.y
            && orientation(first, second, point) > 0){
            ++winding_number;
        }else if(second.y <= point.y && point.y < first.y
            && orientation(first, second, point) < 0){
            --winding_number;
        }
    }
    return winding_number == 0 ? PointLocation::outside
                               : PointLocation::inside;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_ENUMERATION_POINT_SET_POINT_POINT_IN_POLYGON_HPP_INCLUDED
