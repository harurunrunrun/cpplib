#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINTS_IN_HEMISPHERE_WITH_SEED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINTS_IN_HEMISPHERE_WITH_SEED_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include "dot.hpp"
#include "points_in_hemisphere_detail.hpp"
#include "spherical_central_angle.hpp"

inline bool points_in_hemisphere_with_seed(
    const std::vector<Point3>& points,
    std::uint64_t random_seed
){
    if(points.empty()) return true;
    std::vector<Point3> unit;
    unit.reserve(points.size());
    for(const Point3& point: points){
        unit.push_back(geometry3d_spherical_detail::unit_direction(
            point, "invalid point in hemisphere containment"
        ));
    }

    const long double tolerance = 256.0L
        * std::numeric_limits<long double>::epsilon();
    std::vector<std::size_t> order(unit.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::mt19937_64 random_engine(
        random_seed ^ static_cast<std::uint64_t>(unit.size())
    );
    std::shuffle(order.begin(), order.end(), random_engine);

    const auto accepts = [&](const Point3& raw_normal){
        const long double length = std::hypot(
            raw_normal.x, raw_normal.y, raw_normal.z
        );
        if(!(length > 0.0L) || !std::isfinite(length)) return false;
        const Point3 normal = raw_normal / length;
        for(const Point3& point: unit){
            if(dot(normal, point) < -tolerance) return false;
        }
        return true;
    };

    // Scale every possible nonzero normal so that one coordinate is +1 or -1
    // and the other coordinates lie in [-1, 1].  Seidel's randomized
    // incremental LP solves each resulting two-variable problem in expected
    // linear time.
    for(std::size_t fixed = 0; fixed < 3; ++fixed){
        for(const long double sign: {-1.0L, 1.0L}){
            const auto candidate =
                points_in_hemisphere_detail::fixed_coordinate_candidate(
                    unit, order, fixed, sign, tolerance * 0.5L
                );
            if(candidate && accepts(*candidate)) return true;
        }
    }
    return false;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINTS_IN_HEMISPHERE_WITH_SEED_HPP_INCLUDED
