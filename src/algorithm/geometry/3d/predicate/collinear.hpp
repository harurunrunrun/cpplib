#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_COLLINEAR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_COLLINEAR_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "parallel.hpp"

inline bool collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return parallel(second - first, third - first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_COLLINEAR_HPP_INCLUDED
