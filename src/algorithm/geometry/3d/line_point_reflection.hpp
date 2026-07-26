#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_POINT_REFLECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_POINT_REFLECTION_HPP_INCLUDED

#include "line3.hpp"
#include "line_point_projection.hpp"
#include "point3.hpp"
#include "reflection_detail.hpp"

inline Point3 reflection(const Line3& line, const Point3& point){
    return geometry3d_reflection_detail::reflect_from_projection(
        point, projection(line, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_POINT_REFLECTION_HPP_INCLUDED
