#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "adaptive_orient3d.hpp"
#include "distance.hpp"
#include "is_finite.hpp"
#include "polygon3_contains.hpp"
#include "polygon3_normal.hpp"
#include "projection.hpp"

namespace geometry3d_polygon_distance_detail{

struct NormalizedInput{
    Polygon3 polygon;
    Point3 point;
    long double local_scale = 1.0L;
};

inline bool exactly_non_collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    using namespace geometry3d_adaptive_detail;
    const std::array<ExactDyadic, 3> first_direction{
        subtract(exact_dyadic(second.x), exact_dyadic(first.x)),
        subtract(exact_dyadic(second.y), exact_dyadic(first.y)),
        subtract(exact_dyadic(second.z), exact_dyadic(first.z)),
    };
    const std::array<ExactDyadic, 3> second_direction{
        subtract(exact_dyadic(third.x), exact_dyadic(first.x)),
        subtract(exact_dyadic(third.y), exact_dyadic(first.y)),
        subtract(exact_dyadic(third.z), exact_dyadic(first.z)),
    };
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        const std::size_t next = (coordinate + 1) % 3;
        const std::size_t last = (coordinate + 2) % 3;
        if(sign(subtract(
            multiply(first_direction[next], second_direction[last]),
            multiply(first_direction[last], second_direction[next])
        )) != 0){
            return true;
        }
    }
    return false;
}

inline NormalizedInput normalize(const Polygon3& polygon, const Point3& point){
    if(polygon.size() < 3)[[unlikely]]{
        throw std::invalid_argument("a polygon needs at least three vertices");
    }
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("polygon distance requires a finite point");
    }
    long double local_scale = 0.0L;
    for(const Point3& vertex: polygon){
        if(!geometry3d_is_finite(vertex))[[unlikely]]{
            throw std::invalid_argument(
                "polygon distance requires finite vertices"
            );
        }
        const Geometry3DNormalizedDifference difference =
            geometry3d_normalized_difference(
                vertex, polygon.front()
            );
        if(difference.value.x != 0.0L || difference.value.y != 0.0L
            || difference.value.z != 0.0L){
            local_scale = std::max(local_scale, difference.scale);
        }
    }
    if(local_scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("a polygon needs nonzero area");
    }

    Polygon3 normalized;
    normalized.reserve(polygon.size());
    for(const Point3& vertex: polygon){
        normalized.push_back(geometry3d_safe_normalized_difference(
            vertex, polygon.front(), local_scale
        ));
    }
    const Point3 normalized_point =
        geometry3d_safe_normalized_difference(
            point, polygon.front(), local_scale
        );
    if(!geometry3d_is_finite(normalized_point))[[unlikely]]{
        throw std::overflow_error("normalized polygon distance overflows");
    }

    std::size_t second = polygon.size();
    for(std::size_t index = 1; index < polygon.size(); ++index){
        if(polygon[index].x != polygon.front().x
            || polygon[index].y != polygon.front().y
            || polygon[index].z != polygon.front().z){
            second = index;
            break;
        }
    }
    std::size_t third = polygon.size();
    if(second != polygon.size()){
        for(std::size_t index = second + 1; index < polygon.size(); ++index){
            if(exactly_non_collinear(
                polygon.front(), polygon[second], polygon[index]
            )){
                third = index;
                break;
            }
        }
    }
    if(third == polygon.size())[[unlikely]]{
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
    return {normalized, normalized_point, local_scale};
}

inline long double restore_scale(
    long double value,
    long double local_scale
){
    if(value == 0.0L) return 0.0L;
    int value_exponent = 0;
    int local_exponent = 0;
    const long double mantissa =
        std::frexp(value, &value_exponent)
        * std::frexp(local_scale, &local_exponent);
    const long double result = std::ldexp(
        mantissa, value_exponent + local_exponent
    );
    if(!std::isfinite(result) || result == 0.0L)[[unlikely]]{
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
        result, input.local_scale
    );
}
