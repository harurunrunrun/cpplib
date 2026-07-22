#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CIRCLE_CIRCLE_CROSS_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CIRCLE_CIRCLE_CROSS_POINTS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "../predicate/circle_relation.hpp"
#include "../point/rotate90.hpp"

inline std::vector<Point> circle_circle_cross_points(
    const Circle& first,
    const Circle& second
){
    const int relation = circle_relation(first, second);
    if(relation == CIRCLE_COINCIDENT){
        if(first.radius == 0.0L && second.radius == 0.0L){
            return {first.center};
        }
        throw std::domain_error(
            "coincident circles have infinitely many intersections"
        );
    }
    if(relation == CIRCLE_INSIDE || relation == CIRCLE_SEPARATE) return {};
    const long double center_distance =
        distance(first.center, second.center);
    const long double along = (
        first.radius * first.radius - second.radius * second.radius +
        center_distance * center_distance
    ) / (2.0L * center_distance);
    const long double height_squared =
        first.radius * first.radius - along * along;
    const int height_sign = circle_numeric_detail::scaled_sign(
        height_squared,
        first.radius * first.radius + along * along
    );
    if(height_sign < 0) return {};
    const Point direction =
        (second.center - first.center) / center_distance;
    const Point base = first.center + direction * along;
    if(height_sign == 0) return {base};
    const long double height =
        std::sqrt(std::max<long double>(0.0L, height_squared));
    std::vector<Point> result = {
        base - rotate90(direction) * height,
        base + rotate90(direction) * height,
    };
    std::sort(result.begin(), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CIRCLE_CIRCLE_CROSS_POINTS_HPP_INCLUDED
