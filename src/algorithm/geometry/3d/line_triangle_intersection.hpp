#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_TRIANGLE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_TRIANGLE_INTERSECTION_HPP_INCLUDED

#include <array>
#include <variant>
#include <vector>

#include "base.hpp"
#include "contains.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_zero_vector.hpp"
#include "line3_direction.hpp"
#include "line_plane_intersection.hpp"
#include "line_segment_intersection.hpp"
#include "linear_intersection3.hpp"
#include "on_plane.hpp"
#include "triangle_area_vector.hpp"

inline LinearIntersection3 line_triangle_intersection(
    const Line3& line,
    const Triangle3& triangle
){
    const Point3 direction = line3_direction(line);
    const Point3 area_vector = triangle_area_vector(triangle);
    if(is_zero_vector(area_vector))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const Plane3 plane{triangle.a, area_vector};
    if(geometry3d_sign(dot(direction, area_vector)) != 0){
        const Point3 point = line_plane_intersection(line, plane);
        return contains(triangle, point) ? LinearIntersection3{point}
            : LinearIntersection3{std::monostate{}};
    }
    if(!on_plane(plane, line.a)) return std::monostate{};

    const std::array<Segment3, 3> edges{{
        {triangle.a, triangle.b},
        {triangle.b, triangle.c},
        {triangle.c, triangle.a},
    }};
    std::vector<Point3> points;
    for(const Segment3& edge: edges){
        const LinearIntersection3 intersection =
            line_segment_intersection(line, edge);
        if(const auto* overlap_segment = std::get_if<Segment3>(&intersection)){
            return *overlap_segment;
        }
        if(const auto* point = std::get_if<Point3>(&intersection)){
            bool duplicate = false;
            for(const Point3& existing: points){
                if(existing == *point){ duplicate = true; break; }
            }
            if(!duplicate) points.push_back(*point);
        }
    }
    if(points.empty()) return std::monostate{};
    if(points.size() == 1) return points.front();
    return Segment3{points[0], points[1]};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_TRIANGLE_INTERSECTION_HPP_INCLUDED
