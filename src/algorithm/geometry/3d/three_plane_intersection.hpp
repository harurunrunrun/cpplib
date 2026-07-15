#pragma once

#include <variant>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "line_plane_intersection.hpp"
#include "on_plane.hpp"
#include "parallel.hpp"
#include "plane3_unit_normal.hpp"
#include "plane_plane_intersection.hpp"

using ThreePlaneIntersection3 =
    std::variant<std::monostate, Point3, Line3, Plane3>;

inline ThreePlaneIntersection3 three_plane_intersection(
    const Plane3& first,
    const Plane3& second,
    const Plane3& third
){
    const Point3 first_normal = plane3_unit_normal(first);
    const Point3 second_normal = plane3_unit_normal(second);
    const Point3 third_normal = plane3_unit_normal(third);
    if(parallel(first_normal, second_normal)){
        if(!on_plane(first, second.point)) return std::monostate{};
        if(parallel(first_normal, third_normal)){
            return on_plane(first, third.point)
                ? ThreePlaneIntersection3{first}
                : ThreePlaneIntersection3{std::monostate{}};
        }
        return plane_plane_intersection(first, third);
    }
    const Line3 line = plane_plane_intersection(first, second);
    const Point3 direction = line.b - line.a;
    if(geometry3d_sign(dot(direction, third_normal)) == 0){
        return on_plane(third, line.a)
            ? ThreePlaneIntersection3{line}
            : ThreePlaneIntersection3{std::monostate{}};
    }
    return line_plane_intersection(line, third);
}
