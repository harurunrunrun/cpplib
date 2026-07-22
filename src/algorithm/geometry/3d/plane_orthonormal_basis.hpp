#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_ORTHONORMAL_BASIS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_ORTHONORMAL_BASIS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "cross.hpp"
#include "plane3_unit_normal.hpp"
#include "unit.hpp"

inline std::pair<Point3, Point3> plane_orthonormal_basis(const Plane3& plane){
    const Point3 normal = plane3_unit_normal(plane);
    Point3 first;
    if(std::abs(normal.x) <= std::abs(normal.y) &&
        std::abs(normal.x) <= std::abs(normal.z)){
        first = unit(cross(normal, {1, 0, 0}));
    }else if(std::abs(normal.y) <= std::abs(normal.z)){
        first = unit(cross(normal, {0, 1, 0}));
    }else{
        first = unit(cross(normal, {0, 0, 1}));
    }
    return {first, cross(normal, first)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_ORTHONORMAL_BASIS_HPP_INCLUDED
