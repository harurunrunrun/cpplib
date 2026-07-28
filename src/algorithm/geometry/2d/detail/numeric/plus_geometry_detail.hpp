#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DETAIL_NUMERIC_PLUS_GEOMETRY_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DETAIL_NUMERIC_PLUS_GEOMETRY_DETAIL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../scalar/point_point/cross.hpp"
#include "../../scalar/point_point/dot.hpp"
#include "../../type/definition/segment_type.hpp"

namespace plus_geometry_detail{

inline long double scale(const Point& point){
    return std::max({1.0L, std::abs(point.x), std::abs(point.y)});
}

inline int sign(long double value, long double magnitude = 1.0L){
    const long double tolerance =
        128.0L * std::numeric_limits<long double>::epsilon()
        * std::max(1.0L, magnitude);
    return (value > tolerance) - (value < -tolerance);
}

inline int orientation(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point left = second - first;
    const Point right = third - first;
    return sign(
        cross(left, right),
        std::abs(left.x * right.y) + std::abs(left.y * right.x)
    );
}

inline bool close(const Point& first, const Point& second){
    return sign(
        first.x - second.x,
        std::max(scale(first), scale(second))
    ) == 0 && sign(
        first.y - second.y,
        std::max(scale(first), scale(second))
    ) == 0;
}

inline long double squared_distance(
    const Point& first,
    const Point& second
){
    const Point difference = first - second;
    return dot(difference, difference);
}

inline long double signed_area_twice(const std::vector<Point>& polygon){
    long double answer = 0.0L;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        answer += cross(
            polygon[index],
            polygon[(index + 1) % polygon.size()]
        );
    }
    return answer;
}

inline int point_in_polygon(
    const std::vector<Point>& polygon,
    const Point& point
){
    bool inside = false;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % polygon.size()];
        if(orientation(first, second, point) == 0
            && sign(
                dot(point - first, point - second),
                squared_distance(first, second)
            ) <= 0){
            return 1;
        }
        const bool crosses = (first.y > point.y) != (second.y > point.y);
        if(crosses){
            const long double intersection_x = first.x
                + (second.x - first.x) * (point.y - first.y)
                    / (second.y - first.y);
            if(sign(
                intersection_x - point.x,
                std::max({scale(first), scale(second), scale(point)})
            ) > 0){
                inside = !inside;
            }
        }
    }
    return inside ? 2 : 0;
}

inline bool segment_parameter(
    const Segment& first,
    const Segment& second,
    long double& first_parameter,
    long double& second_parameter
){
    const Point first_direction = first.b - first.a;
    const Point second_direction = second.b - second.a;
    const long double denominator = cross(first_direction, second_direction);
    const long double magnitude =
        std::abs(first_direction.x * second_direction.y)
        + std::abs(first_direction.y * second_direction.x);
    if(sign(denominator, magnitude) == 0) return false;
    const Point offset = second.a - first.a;
    first_parameter = cross(offset, second_direction) / denominator;
    second_parameter = cross(offset, first_direction) / denominator;
    return sign(first_parameter, 1.0L) >= 0
        && sign(first_parameter - 1.0L, 1.0L) <= 0
        && sign(second_parameter, 1.0L) >= 0
        && sign(second_parameter - 1.0L, 1.0L) <= 0;
}

inline Point interpolate(const Segment& segment, long double parameter){
    return segment.a + (segment.b - segment.a) * parameter;
}

inline std::vector<Point> normalized_polygon(std::vector<Point> polygon){
    if(polygon.size() >= 2 && close(polygon.front(), polygon.back())){
        polygon.pop_back();
    }
    std::vector<Point> result;
    result.reserve(polygon.size());
    for(const Point& point: polygon){
        if(result.empty() || !close(result.back(), point)){
            result.push_back(point);
        }
    }
    if(result.size() >= 2 && close(result.front(), result.back())){
        result.pop_back();
    }
    if(result.size() >= 3 && signed_area_twice(result) < 0.0L){
        std::reverse(result.begin(), result.end());
    }
    return result;
}

inline void validate_finite(const Point& point){
    if(!std::isfinite(point.x) || !std::isfinite(point.y)){
        throw std::invalid_argument("geometry coordinates must be finite");
    }
}

}  // namespace plus_geometry_detail

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DETAIL_NUMERIC_PLUS_GEOMETRY_DETAIL_HPP_INCLUDED
