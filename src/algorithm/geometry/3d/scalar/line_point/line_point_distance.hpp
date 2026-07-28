#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_POINT_LINE_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_POINT_LINE_POINT_DISTANCE_HPP_INCLUDED

#include "../../type/definition/line3.hpp"
#include "../../point/line_point/line_point_projection.hpp"
#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../type/definition/point3.hpp"

inline long double distance(const Line3& line, const Point3& point){
    return geometry3d_linear_closest_detail::checked_distance(
        point, projection(line, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_POINT_LINE_POINT_DISTANCE_HPP_INCLUDED
