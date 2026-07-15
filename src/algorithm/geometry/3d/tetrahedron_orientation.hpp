#pragma once

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
