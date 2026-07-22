#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TETRAHEDRON_ORIENTATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TETRAHEDRON_ORIENTATION_HPP_INCLUDED

#include "adaptive_orient3d.hpp"
#include "base.hpp"

inline int tetrahedron_orientation(const Tetrahedron3& tetrahedron){
    return adaptive_orient3d(
        tetrahedron.a,
        tetrahedron.b,
        tetrahedron.c,
        tetrahedron.d
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TETRAHEDRON_ORIENTATION_HPP_INCLUDED
