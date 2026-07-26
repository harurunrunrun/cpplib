#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_COMMON_TANGENTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_COMMON_TANGENTS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <stdexcept>
#include <vector>

#include "norm.hpp"
#include "rotate90.hpp"
#include "same_line.hpp"
#include "validate_circle.hpp"

namespace common_tangents_detail{

inline std::vector<Line> common_tangents_with_sides(
    const Circle& first,
    const Circle& second,
    std::initializer_list<int> same_sides
){
    validate_circle(first);
    validate_circle(second);
    const Point center_difference = second.center - first.center;
    const long double distance_squared = norm(center_difference);
    const bool same_center = circle_numeric_detail::scaled_sign(
        distance_squared,
        distance_squared + first.radius * first.radius
            + second.radius * second.radius
    ) == 0;
    if(same_center &&
       circle_numeric_detail::compare(first.radius, second.radius) == 0){
        throw std::domain_error(
            "coincident circles have infinitely many common tangents"
        );
    }
    if(same_center) return {};
    std::vector<Line> result;
    for(const int same_side: same_sides){
        const long double radius_difference =
            first.radius - same_side * second.radius;
        const long double height_squared =
            distance_squared - radius_difference * radius_difference;
        const int height_sign = circle_numeric_detail::scaled_sign(
            height_squared, distance_squared + radius_difference * radius_difference
        );
        if(height_sign < 0) continue;
        const long double height =
            std::sqrt(std::max<long double>(0.0L, height_squared));
        for(const int sign: {-1, 1}){
            const Point normal = (
                center_difference * radius_difference +
                rotate90(center_difference) * (height * sign)
            ) / distance_squared;
            const Point first_point =
                first.center + normal * first.radius;
            const Point second_point =
                second.center + normal * (same_side * second.radius);
            const long double point_distance_squared =
                norm(first_point - second_point);
            const bool same_point = circle_numeric_detail::scaled_sign(
                point_distance_squared,
                point_distance_squared + first.radius * first.radius
                    + second.radius * second.radius
            ) == 0;
            const Line tangent = same_point
                ? Line{first_point, first_point + rotate90(normal)}
                : Line{first_point, second_point};
            const bool duplicate = std::any_of(
                result.begin(), result.end(),
                [&](const Line& line){ return same_line(line, tangent); }
            );
            if(!duplicate) result.push_back(tangent);
            if(height_sign == 0) break;
        }
    }
    return result;
}

}  // namespace common_tangents_detail

inline std::vector<Line> common_tangents(
    const Circle& first,
    const Circle& second
){
    return common_tangents_detail::common_tangents_with_sides(
        first, second, {-1, 1}
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_COMMON_TANGENTS_HPP_INCLUDED
