#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_PLANE_POINT_PLANE_POINT_REFLECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_PLANE_POINT_PLANE_POINT_REFLECTION_HPP_INCLUDED

#include "../../type/definition/plane3.hpp"
#include "plane_point_projection.hpp"
#include "../../type/definition/point3.hpp"
#include "../../detail/point_point/reflection_detail.hpp"

inline Point3 reflection(const Plane3& plane, const Point3& point){
    return geometry3d_reflection_detail::reflect_from_projection(
        point, projection(plane, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_PLANE_POINT_PLANE_POINT_REFLECTION_HPP_INCLUDED
