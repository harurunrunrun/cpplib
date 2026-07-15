#pragma once

#include <cmath>

#include "convex_polyhedron_signed_volume.hpp"

inline long double convex_polyhedron_volume(const ConvexPolyhedron3& polyhedron){
    return std::abs(convex_polyhedron_signed_volume(polyhedron));
}
