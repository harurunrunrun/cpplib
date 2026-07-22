#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SIGNED_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SIGNED_DISTANCE_HPP_INCLUDED

#include <cmath>
#include <limits>
#include <stdexcept>

#include "../core/geometry_primitives.hpp"
#include "dot.hpp"
#include "../predicate/is_finite.hpp"
#include "../point/plane3_unit_normal.hpp"

inline long double signed_distance(const Plane3& plane, const Point3& point){
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument(
            "signed distance requires a finite point"
        );
    }
    const Point3 normal = plane3_unit_normal(plane);
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(point, plane.point);
    const long double normalized = dot(difference.value, normal);
    if(normalized == 0.0L) return 0.0L;
    if(std::abs(normalized)
        > std::numeric_limits<long double>::max() / difference.scale){
        throw std::overflow_error(
            "signed distance is not representable"
        );
    }
    const long double result = normalized * difference.scale;
    if(!std::isfinite(result) || result == 0.0L)[[unlikely]]{
        throw std::overflow_error(
            "signed distance is not representable"
        );
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SIGNED_DISTANCE_HPP_INCLUDED
