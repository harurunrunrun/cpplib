#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POINT_POINT_POINT_POINT_COPLANAR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POINT_POINT_POINT_POINT_COPLANAR_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../integer/point_point_point_point/adaptive_orient3d.hpp"

inline bool coplanar(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    return adaptive_orient3d(first, second, third, fourth) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POINT_POINT_POINT_POINT_COPLANAR_HPP_INCLUDED
