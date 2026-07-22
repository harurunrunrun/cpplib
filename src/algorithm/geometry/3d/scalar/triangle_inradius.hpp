#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_INRADIUS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_INRADIUS_HPP_INCLUDED

#include "../core/geometry_primitives.hpp"
#include "../point/triangle_incenter.hpp"

inline long double triangle_inradius(const Triangle3& triangle){
    const auto data =
        geometry3d_triangle_incenter_detail::local_data(triangle);
    return geometry3d_triangle_incenter_detail::restore_length(
        data.normalized_radius, data.scale,
        "3D triangle inradius is not representable"
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_INRADIUS_HPP_INCLUDED
