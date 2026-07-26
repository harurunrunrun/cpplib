#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SIMPLE_POLYGON_TRIANGULATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SIMPLE_POLYGON_TRIANGULATION_HPP_INCLUDED

#include <vector>

#include "ear_clipping_triangulation.hpp"

inline std::vector<EarClippingTriangle> simple_polygon_triangulation(
    const std::vector<Point>& polygon
){
    return ear_clipping_triangulation(polygon);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SIMPLE_POLYGON_TRIANGULATION_HPP_INCLUDED
