#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_POINT_SET_EAR_CLIPPING_TRIANGULATION_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_POINT_SET_EAR_CLIPPING_TRIANGULATION_POINTS_HPP_INCLUDED

#include <vector>

#include "../../type/definition/ear_clipping_triangle.hpp"
#include "../../detail/numeric/ear_clipping_triangulation_detail.hpp"

inline std::vector<EarClippingTriangle> ear_clipping_triangulation(
    const std::vector<Point>& polygon
){
    return simple_polygon_triangulation_detail::triangulate(polygon);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_POINT_SET_EAR_CLIPPING_TRIANGULATION_POINTS_HPP_INCLUDED
