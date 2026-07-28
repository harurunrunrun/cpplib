#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_POINT_SET_POLYGON_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_POINT_SET_POLYGON_AREA_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "../../type/definition/geometry_types.hpp"

namespace integer_geometry{

inline Rational polygon_twice_signed_area(const std::vector<Point>& polygon){
    Rational result = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % polygon.size()];
        result += first.x * second.y - first.y * second.x;
    }
    return result;
}

inline Rational polygon_twice_area(const std::vector<Point>& polygon){
    return abs(polygon_twice_signed_area(polygon));
}

inline Rational polygon_signed_area(const std::vector<Point>& polygon){
    return polygon_twice_signed_area(polygon) / Rational{2};
}

inline Rational polygon_area(const std::vector<Point>& polygon){
    return polygon_twice_area(polygon) / Rational{2};
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_POINT_SET_POLYGON_AREA_HPP_INCLUDED
