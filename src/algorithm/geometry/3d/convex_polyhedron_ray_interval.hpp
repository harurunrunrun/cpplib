#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_RAY_INTERVAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_RAY_INTERVAL_HPP_INCLUDED

#include <limits>
#include <optional>
#include <utility>

#include "convex_polyhedron_line_interval.hpp"

inline std::optional<std::pair<long double, long double>>
convex_polyhedron_ray_interval(
    const ConvexPolyhedron3& polyhedron,
    const Ray3& ray
){
    return convex_polyhedron_interval_detail::clip(
        polyhedron, ray.origin, ray.through, 0.0L,
        std::numeric_limits<long double>::infinity()
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_RAY_INTERVAL_HPP_INCLUDED
