#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POLYHEDRON_VOLUME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POLYHEDRON_VOLUME_HPP_INCLUDED

#include <cmath>

#include "polyhedron_signed_volume.hpp"

inline long double polyhedron_volume(const Polyhedron3& polyhedron){
    return std::abs(polyhedron_signed_volume(polyhedron));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POLYHEDRON_VOLUME_HPP_INCLUDED
