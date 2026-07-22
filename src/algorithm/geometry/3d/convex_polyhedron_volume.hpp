#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_VOLUME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_VOLUME_HPP_INCLUDED

#include <cmath>

#include "convex_polyhedron_signed_volume.hpp"

inline long double convex_polyhedron_volume(const ConvexPolyhedron3& polyhedron){
    return std::abs(convex_polyhedron_signed_volume(polyhedron));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_VOLUME_HPP_INCLUDED
