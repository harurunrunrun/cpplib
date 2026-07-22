#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE3_UNIT_NORMAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE3_UNIT_NORMAL_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "unit.hpp"

inline Point3 plane3_unit_normal(const Plane3& plane){
    return unit(plane.normal);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE3_UNIT_NORMAL_HPP_INCLUDED
