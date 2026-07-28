#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POINT_POINT_POINT_COLLINEAR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POINT_POINT_POINT_COLLINEAR_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../point_point/parallel.hpp"

inline bool collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return parallel(second - first, third - first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POINT_POINT_POINT_COLLINEAR_HPP_INCLUDED
