#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_TANGENT_LINES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_TANGENT_LINES_HPP_INCLUDED

#include <stdexcept>
#include <vector>

#include "../point/rotate90.hpp"
#include "../point_collection/tangent_points.hpp"
#include "../predicate/validate_circle.hpp"

inline std::vector<Line> tangent_lines(
    const Circle& circle,
    const Point& point
){
    validate_circle(circle);
    if(circle.radius == 0.0L){
        if(point.x == circle.center.x && point.y == circle.center.y){
            throw std::domain_error(
                "a point circle has no unique tangent at its center"
            );
        }
        return {{point, circle.center}};
    }
    std::vector<Line> result;
    for(const Point& tangent_point: tangent_points(circle, point)){
        if(tangent_point.x == point.x && tangent_point.y == point.y){
            result.push_back({
                tangent_point,
                tangent_point + rotate90(tangent_point - circle.center),
            });
        }else{
            result.push_back({point, tangent_point});
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_TANGENT_LINES_HPP_INCLUDED
