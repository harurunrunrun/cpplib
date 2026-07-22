#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_NORM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_NORM_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "dot.hpp"

inline long double norm(const Point3& point){ return dot(point, point); }

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_NORM_HPP_INCLUDED
