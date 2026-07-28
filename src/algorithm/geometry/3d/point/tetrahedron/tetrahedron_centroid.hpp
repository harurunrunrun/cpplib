#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TETRAHEDRON_TETRAHEDRON_CENTROID_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TETRAHEDRON_TETRAHEDRON_CENTROID_HPP_INCLUDED

#include "../../detail/point/centroid_detail.hpp"
#include "../../type/definition/point3.hpp"
#include "../../type/definition/tetrahedron3.hpp"

inline Point3 centroid(const Tetrahedron3& tetrahedron){
    return geometry3d_centroid_detail::centroid<4>({
        tetrahedron.a, tetrahedron.b, tetrahedron.c, tetrahedron.d,
    });
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TETRAHEDRON_TETRAHEDRON_CENTROID_HPP_INCLUDED
