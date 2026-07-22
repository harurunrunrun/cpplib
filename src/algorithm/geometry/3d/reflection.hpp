#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_REFLECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_REFLECTION_HPP_INCLUDED

#include "base.hpp"
#include "projection.hpp"

namespace geometry3d_reflection_detail{

inline Point3 reflect_from_projection(
    const Point3& point,
    const Point3& projected
){
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(projected, point);
    return geometry3d_linear_closest_detail::restore(
        difference.value * 2.0L, point, difference.scale
    );
}

}  // namespace geometry3d_reflection_detail

inline Point3 reflection(const Line3& line, const Point3& point){
    return geometry3d_reflection_detail::reflect_from_projection(
        point, projection(line, point)
    );
}

inline Point3 reflection(const Plane3& plane, const Point3& point){
    return geometry3d_reflection_detail::reflect_from_projection(
        point, projection(plane, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_REFLECTION_HPP_INCLUDED
