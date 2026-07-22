#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_SEGMENT_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "../scalar/abs.hpp"
#include "../scalar/dot.hpp"
#include "geometry3d_sign.hpp"
#include "parallel.hpp"

inline bool on_segment(const Segment3& segment, const Point3& point){
    const Point3 direction = segment.b - segment.a;
    if(geometry3d_sign(abs(direction)) == 0) return point == segment.a;
    return parallel(direction, point - segment.a) &&
        geometry3d_sign(dot(point - segment.a, point - segment.b)) <= 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_SEGMENT_HPP_INCLUDED
