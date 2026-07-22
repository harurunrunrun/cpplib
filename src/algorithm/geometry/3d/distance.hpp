#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DISTANCE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "closest_point.hpp"
#include "closest_points.hpp"
#include "projection.hpp"
#include "signed_distance.hpp"

inline long double distance(const Point3& left, const Point3& right){
    return geometry3d_linear_closest_detail::checked_distance(left, right);
}

inline long double distance(const Line3& line, const Point3& point){
    return geometry3d_linear_closest_detail::checked_distance(
        point, projection(line, point)
    );
}

inline long double distance(const Ray3& ray, const Point3& point){
    return geometry3d_linear_closest_detail::checked_distance(
        point, closest_point(ray, point)
    );
}

inline long double distance(const Segment3& segment, const Point3& point){
    return geometry3d_linear_closest_detail::checked_distance(
        point, closest_point(segment, point)
    );
}

inline long double distance(const Plane3& plane, const Point3& point){
    return std::abs(signed_distance(plane, point));
}

inline long double distance(const Line3& first, const Line3& second){
    using namespace geometry3d_linear_closest_detail;
    const Geometry3DNormalizedDifference first_difference =
        geometry3d_normalized_difference(first.b, first.a);
    const Geometry3DNormalizedDifference second_difference =
        geometry3d_normalized_difference(second.b, second.a);
    if(!nonzero(first_difference) || !nonzero(second_difference))
        [[unlikely]]{
        throw std::invalid_argument("degenerate 3D linear primitive");
    }
    const Point3 first_direction = unit(first_difference.value);
    const Point3 second_direction = unit(second_difference.value);
    const Geometry3DNormalizedDifference origin_difference =
        geometry3d_normalized_difference(first.a, second.a);
    const Point3 product = cross(first_direction, second_direction);
    const long double sine = std::hypot(
        product.x, product.y, product.z
    );
    long double normalized_distance;
    if(sine == 0.0L){
        const Point3 perpendicular = cross(
            origin_difference.value, first_direction
        );
        normalized_distance = std::hypot(
            perpendicular.x, perpendicular.y, perpendicular.z
        );
    }else{
        normalized_distance = std::abs(dot(
            origin_difference.value, product / sine
        ));
    }
    return checked_scale_product(
        normalized_distance,
        origin_difference.scale,
        "3D line distance is not representable"
    );
}

inline long double distance(const Segment3& first, const Segment3& second){
    const auto [left, right] = closest_points(first, second);
    return geometry3d_linear_closest_detail::checked_distance(left, right);
}

inline long double distance(const Triangle3& triangle, const Point3& point){
    return geometry3d_linear_closest_detail::checked_distance(
        point, closest_point(triangle, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DISTANCE_HPP_INCLUDED
