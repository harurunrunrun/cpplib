#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_POINT_POINT_SCALAR_TRIPLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_POINT_POINT_SCALAR_TRIPLE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point/point_point/cross.hpp"
#include "../point_point/dot.hpp"

inline long double scalar_triple(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return dot(first, cross(second, third));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_POINT_POINT_SCALAR_TRIPLE_HPP_INCLUDED
