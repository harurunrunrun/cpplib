#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "adaptive_orient3d.hpp"
#include "cross.hpp"
#include "distance.hpp"
#include "is_finite.hpp"
#include "polygon3_contains.hpp"
#include "polygon3_normal.hpp"
#include "projection.hpp"

namespace geometry3d_polygon_distance_detail{

struct NormalizedInput{
    Polygon3 polygon;
    Point3 point;
    long double coordinate_scale = 1.0L;
    long double local_scale = 1.0L;
};

inline long double normalized_difference(
    long double left,
    long double right,
    long double scale
){
    if(std::signbit(left) == std::signbit(right)){
        return (left - right) / scale;
    }
    return left / scale - right / scale;
}

inline Point3 normalized_difference(
    const Point3& left,
    const Point3& right,
    long double scale
){
    return {
        normalized_difference(left.x, right.x, scale),
        normalized_difference(left.y, right.y, scale),
        normalized_difference(left.z, right.z, scale),
    };
}

inline NormalizedInput normalize(const Polygon3& polygon, const Point3& point){
    if(polygon.size() < 3)[[unlikely]]{
        throw std::invalid_argument("a polygon needs at least three vertices");
    }
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("polygon distance requires a finite point");
    }
    long double coordinate_scale = std::max({
        std::abs(point.x), std::abs(point.y), std::abs(point.z),
    });
    for(const Point3& vertex: polygon){
        if(!geometry3d_is_finite(vertex))[[unlikely]]{
            throw std::invalid_argument(
                "polygon distance requires finite vertices"
            );
        }
        coordinate_scale = std::max({
            coordinate_scale,
            std::abs(vertex.x), std::abs(vertex.y), std::abs(vertex.z),
        });
    }
    if(coordinate_scale == 0.0L) coordinate_scale = 1.0L;

    Polygon3 normalized;
    normalized.reserve(polygon.size());
    long double local_scale = 0.0L;
    for(const Point3& vertex: polygon){
        const Point3 offset = normalized_difference(
            vertex, polygon.front(), coordinate_scale
        );
        normalized.push_back(offset);
        local_scale = std::max({
            local_scale,
            std::abs(offset.x), std::abs(offset.y), std::abs(offset.z),
        });
    }
    if(local_scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("a polygon needs nonzero area");
    }
    for(Point3& vertex: normalized) vertex /= local_scale;
    const Point3 normalized_point = normalized_difference(
        point, polygon.front(), coordinate_scale
    ) / local_scale;
    if(!geometry3d_is_finite(normalized_point))[[unlikely]]{
        throw std::overflow_error("normalized polygon distance overflows");
    }

    std::size_t second = 0;
    std::size_t third = 0;
    bool found_plane = false;
    for(std::size_t left = 1; left < normalized.size() && !found_plane; ++left){
        for(std::size_t right = left + 1; right < normalized.size(); ++right){
            const Point3 normal = cross(normalized[left], normalized[right]);
            if(normal.x != 0.0L || normal.y != 0.0L || normal.z != 0.0L){
                second = left;
                third = right;
                found_plane = true;
                break;
            }
        }
    }
    if(!found_plane)[[unlikely]]{
        throw std::invalid_argument("a polygon needs nonzero area");
    }
    for(const Point3& vertex: normalized){
        if(adaptive_orient3d(
            normalized.front(), normalized[second], normalized[third], vertex
        ) != 0)[[unlikely]]{
            throw std::invalid_argument("polygon vertices must be coplanar");
        }
    }
    // This also rejects a zero signed area caused by a degenerate boundary.
    (void)polygon3_normal(normalized);
    return {normalized, normalized_point, coordinate_scale, local_scale};
}

inline long double restore_scale(
    long double value,
    long double local_scale,
    long double coordinate_scale
){
    if(value == 0.0L) return 0.0L;
    int value_exponent = 0;
    int local_exponent = 0;
    int coordinate_exponent = 0;
    const long double mantissa =
        std::frexp(value, &value_exponent)
        * std::frexp(local_scale, &local_exponent)
        * std::frexp(coordinate_scale, &coordinate_exponent);
    const long double result = std::ldexp(
        mantissa, value_exponent + local_exponent + coordinate_exponent
    );
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("polygon distance is not representable");
    }
    return result;
}

}  // namespace geometry3d_polygon_distance_detail

inline long double polygon3_distance(
    const Polygon3& polygon,
    const Point3& point
){
    const auto input = geometry3d_polygon_distance_detail::normalize(
        polygon, point
    );
    const Plane3 plane{input.polygon.front(), polygon3_normal(input.polygon)};
    const Point3 projected = projection(plane, input.point);
    long double result = std::numeric_limits<long double>::infinity();
    if(polygon3_contains(input.polygon, projected) != 0){
        result = distance(plane, input.point);
    }else{
        for(std::size_t index = 0; index < input.polygon.size(); ++index){
            result = std::min(result, distance(
                Segment3{
                    input.polygon[index],
                    input.polygon[(index + 1) % input.polygon.size()],
                },
                input.point
            ));
        }
    }
    return geometry3d_polygon_distance_detail::restore_scale(
        result, input.local_scale, input.coordinate_scale
    );
}
