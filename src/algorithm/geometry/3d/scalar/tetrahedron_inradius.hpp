#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TETRAHEDRON_INRADIUS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TETRAHEDRON_INRADIUS_HPP_INCLUDED

#include "../core/geometry_primitives.hpp"
#include "../point/tetrahedron_incenter.hpp"

inline long double tetrahedron_inradius(const Tetrahedron3& tetrahedron){
    const auto data =
        geometry3d_tetrahedron_incenter_detail::local_data(tetrahedron);
    return geometry3d_tetrahedron_incenter_detail::restore_length(
        data.normalized_radius, data.scale,
        "tetrahedron inradius is not representable"
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TETRAHEDRON_INRADIUS_HPP_INCLUDED
