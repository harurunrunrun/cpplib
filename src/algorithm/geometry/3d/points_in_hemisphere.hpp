#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINTS_IN_HEMISPHERE_HPP
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINTS_IN_HEMISPHERE_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

#include "spherical_central_angle.hpp"

namespace points_in_hemisphere_detail{

struct LinearConstraint2D{
    long double x = 0.0L;
    long double y = 0.0L;
    long double bound = 0.0L;
};

struct Point2D{
    long double x = 0.0L;
    long double y = 0.0L;
};

inline bool satisfies(
    const LinearConstraint2D& constraint,
    const Point2D& point
){
    return constraint.x * point.x + constraint.y * point.y
        <= constraint.bound;
}

inline bool restrict_parameter(
    long double coefficient,
    long double bound,
    long double tolerance,
    long double& lower,
    long double& upper
){
    if(std::abs(coefficient) <= tolerance){
        return bound >= -tolerance;
    }
    const long double endpoint = bound / coefficient;
    if(coefficient > 0.0L){
        upper = std::min(upper, endpoint);
    }else{
        lower = std::max(lower, endpoint);
    }
    return lower <= upper + tolerance;
}

inline std::optional<Point2D> optimize_on_boundary(
    const LinearConstraint2D& boundary,
    const std::vector<LinearConstraint2D>& constraints,
    const std::vector<std::size_t>& order,
    std::size_t processed,
    long double tolerance
){
    const long double squared_normal =
        boundary.x * boundary.x + boundary.y * boundary.y;
    if(squared_normal <= tolerance * tolerance) return std::nullopt;

    const Point2D origin{
        boundary.x * boundary.bound / squared_normal,
        boundary.y * boundary.bound / squared_normal,
    };
    const Point2D direction{-boundary.y, boundary.x};
    long double lower = -std::numeric_limits<long double>::infinity();
    long double upper = std::numeric_limits<long double>::infinity();

    const auto add_constraint = [&](long double x, long double y,
                                    long double bound){
        return restrict_parameter(
            x * direction.x + y * direction.y,
            bound - x * origin.x - y * origin.y,
            tolerance, lower, upper
        );
    };
    if(!add_constraint(1.0L, 0.0L, 1.0L)
        || !add_constraint(-1.0L, 0.0L, 1.0L)
        || !add_constraint(0.0L, 1.0L, 1.0L)
        || !add_constraint(0.0L, -1.0L, 1.0L)){
        return std::nullopt;
    }
    for(std::size_t index = 0; index < processed; ++index){
        const auto& constraint = constraints[order[index]];
        if(!add_constraint(
            constraint.x, constraint.y, constraint.bound
        )) return std::nullopt;
    }

    constexpr long double objective_x = 1.0L;
    constexpr long double objective_y =
        0.618033988749894848204586834365638118L;
    const long double slope =
        objective_x * direction.x + objective_y * direction.y;
    long double parameter = 0.0L;
    if(slope > tolerance){
        parameter = lower;
    }else if(slope < -tolerance){
        parameter = upper;
    }else{
        parameter = lower / 2.0L + upper / 2.0L;
    }
    if(!std::isfinite(parameter)) return std::nullopt;
    return Point2D{
        origin.x + parameter * direction.x,
        origin.y + parameter * direction.y,
    };
}

inline std::optional<Point2D> feasible_point_2d(
    const std::vector<LinearConstraint2D>& constraints,
    const std::vector<std::size_t>& order,
    long double tolerance
){
    Point2D point{-1.0L, -1.0L};
    for(std::size_t processed = 0;
        processed < order.size();
        ++processed){
        const auto& constraint = constraints[order[processed]];
        if(satisfies(constraint, point)) continue;
        const auto replacement = optimize_on_boundary(
            constraint, constraints, order, processed, tolerance
        );
        if(!replacement) return std::nullopt;
        point = *replacement;
    }
    return point;
}

inline long double coordinate(const Point3& point, std::size_t index){
    if(index == 0) return point.x;
    if(index == 1) return point.y;
    return point.z;
}

inline std::optional<Point3> fixed_coordinate_candidate(
    const std::vector<Point3>& points,
    const std::vector<std::size_t>& order,
    std::size_t fixed_coordinate,
    long double fixed_value,
    long double tolerance
){
    const std::size_t first = (fixed_coordinate + 1) % 3;
    const std::size_t second = (fixed_coordinate + 2) % 3;
    std::vector<LinearConstraint2D> constraints;
    constraints.reserve(points.size());
    for(const Point3& point: points){
        constraints.push_back({
            -coordinate(point, first),
            -coordinate(point, second),
            coordinate(point, fixed_coordinate) * fixed_value + tolerance,
        });
    }
    const auto variables = feasible_point_2d(
        constraints, order, tolerance * 0.25L
    );
    if(!variables) return std::nullopt;
    std::array<long double, 3> embedded{};
    embedded[fixed_coordinate] = fixed_value;
    embedded[first] = variables->x;
    embedded[second] = variables->y;
    return Point3{embedded[0], embedded[1], embedded[2]};
}

}  // namespace points_in_hemisphere_detail

inline bool points_in_hemisphere(const std::vector<Point3>& points){
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
        0xD1B54A32D192ED03ULL
        ^ static_cast<std::uint64_t>(unit.size())
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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINTS_IN_HEMISPHERE_HPP
