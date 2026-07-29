#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_POINT_ON_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_POINT_ON_SEGMENT_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../scalar/point/abs.hpp"
#include "../../scalar/point_point/dot.hpp"
#include "../../integer/scalar/geometry3d_sign.hpp"
#include "../point_point/parallel.hpp"

inline bool on_segment(const Segment3& segment, const Point3& point){
    if(point == segment.a || point == segment.b) return true;
    const Point3 direction = segment.b - segment.a;
    if(geometry3d_sign(abs(direction)) == 0) return false;
    return parallel(direction, point - segment.a) &&
        geometry3d_sign(dot(point - segment.a, point - segment.b)) <= 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_POINT_ON_SEGMENT_HPP_INCLUDED
