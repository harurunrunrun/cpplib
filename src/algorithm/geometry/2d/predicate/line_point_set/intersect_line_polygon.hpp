#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_POINT_SET_INTERSECT_LINE_POLYGON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_POINT_SET_INTERSECT_LINE_POLYGON_HPP_INCLUDED

#include <cmath>
#include <stdexcept>
#include <vector>

#include "../../aggregate/all/geometry_sign.hpp"
#include "../../scalar/point_point/cross.hpp"
#include "../../type/definition/line.hpp"

inline bool intersect_line_polygon(
    const Line& line,
    const std::vector<Point>& polygon
){
    if(!std::isfinite(line.a.x) || !std::isfinite(line.a.y)
        || !std::isfinite(line.b.x) || !std::isfinite(line.b.y))[[unlikely]]{
        throw std::invalid_argument("non-finite line");
    }
    const Point direction = line.b - line.a;
    if(geometry_sign(direction.x) == 0
        && geometry_sign(direction.y) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }

    bool positive = false;
    bool negative = false;
    for(const Point& point: polygon){
        if(!std::isfinite(point.x) || !std::isfinite(point.y))[[unlikely]]{
            throw std::invalid_argument("non-finite polygon vertex");
        }
        const int side = geometry_sign(cross(direction, point - line.a));
        if(side == 0) return true;
        positive = positive || side > 0;
        negative = negative || side < 0;
        if(positive && negative) return true;
    }
    return false;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_POINT_SET_INTERSECT_LINE_POLYGON_HPP_INCLUDED
