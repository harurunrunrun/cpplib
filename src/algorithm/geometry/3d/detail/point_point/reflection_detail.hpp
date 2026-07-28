#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_POINT_POINT_REFLECTION_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_POINT_POINT_REFLECTION_DETAIL_HPP_INCLUDED

#include "../scalar/linear_closest_detail.hpp"
#include "../../type/definition/point3.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_POINT_POINT_REFLECTION_DETAIL_HPP_INCLUDED
