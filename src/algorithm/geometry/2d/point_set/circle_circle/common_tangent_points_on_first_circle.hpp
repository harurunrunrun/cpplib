#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_SET_CIRCLE_CIRCLE_COMMON_TANGENT_POINTS_ON_FIRST_CIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_SET_CIRCLE_CIRCLE_COMMON_TANGENT_POINTS_ON_FIRST_CIRCLE_HPP_INCLUDED

#include <algorithm>
#include <vector>

#include "../../line_set/circle_circle/common_tangents.hpp"
#include "../../point/line_point/projection.hpp"

inline std::vector<Point> common_tangent_points_on_first_circle(
    const Circle& first,
    const Circle& second
){
    const std::vector<Line> tangents = common_tangents(first, second);
    std::vector<Point> points;
    points.reserve(tangents.size());
    for(const Line& tangent: tangents){
        points.push_back(projection(tangent, first.center));
    }
    std::sort(points.begin(), points.end());
    return points;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_SET_CIRCLE_CIRCLE_COMMON_TANGENT_POINTS_ON_FIRST_CIRCLE_HPP_INCLUDED
