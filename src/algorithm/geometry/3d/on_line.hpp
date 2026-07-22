#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ON_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ON_LINE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "line3_direction.hpp"
#include "parallel.hpp"

inline bool on_line(const Line3& line, const Point3& point){
    return parallel(line3_direction(line), point - line.a);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ON_LINE_HPP_INCLUDED
