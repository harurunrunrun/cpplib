#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INDEX_POINT_SET_POINT_POLYGON_SUPPORT_VERTEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INDEX_POINT_SET_POINT_POLYGON_SUPPORT_VERTEX_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../scalar/point_point/dot.hpp"

inline constexpr std::size_t POLYGON_SUPPORT_VERTEX_NPOS =
    std::numeric_limits<std::size_t>::max();

inline std::size_t polygon_support_vertex(
    const std::vector<Point>& polygon,
    const Point& direction
){
    if(!std::isfinite(direction.x) || !std::isfinite(direction.y))[[unlikely]]{
        throw std::invalid_argument("non-finite polygon support direction");
    }
    if(polygon.empty()) return POLYGON_SUPPORT_VERTEX_NPOS;

    std::size_t result = 0;
    long double best = 0.0L;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& point = polygon[index];
        if(!std::isfinite(point.x) || !std::isfinite(point.y))[[unlikely]]{
            throw std::invalid_argument("non-finite polygon vertex");
        }
        const long double value = dot(point, direction);
        if(index == 0 || value > best){
            result = index;
            best = value;
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INDEX_POINT_SET_POINT_POLYGON_SUPPORT_VERTEX_HPP_INCLUDED
