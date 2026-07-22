#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_PLANE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_PLANE_INTERSECTION_HPP_INCLUDED

#include <array>
#include <variant>
#include <vector>

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "is_zero_vector.hpp"
#include "planar_intersection3.hpp"
#include "signed_distance.hpp"
#include "triangle_area_vector.hpp"

inline PlanarIntersection3 triangle_plane_intersection(
    const Triangle3& triangle,
    const Plane3& plane
){
    if(is_zero_vector(triangle_area_vector(triangle)))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const std::array<Point3, 3> vertices{
        triangle.a, triangle.b, triangle.c
    };
    std::array<long double, 3> distances{};
    std::array<int, 3> signs{};
    bool all_on_plane = true;
    for(std::size_t index = 0; index < 3; ++index){
        distances[index] = signed_distance(plane, vertices[index]);
        signs[index] = geometry3d_sign(distances[index]);
        if(signs[index] != 0) all_on_plane = false;
    }
    if(all_on_plane) return Polygon3{triangle.a, triangle.b, triangle.c};

    std::vector<Point3> points;
    const auto add = [&](const Point3& point){
        for(const Point3& existing: points) if(existing == point) return;
        points.push_back(point);
    };
    for(std::size_t index = 0; index < 3; ++index){
        const std::size_t next = (index + 1) % 3;
        if(signs[index] == 0) add(vertices[index]);
        if(signs[index] * signs[next] < 0){
            const long double parameter = distances[index] /
                (distances[index] - distances[next]);
            add(vertices[index] + (vertices[next] - vertices[index]) * parameter);
        }
    }
    if(points.empty()) return std::monostate{};
    if(points.size() == 1) return points.front();
    return Segment3{points[0], points[1]};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_PLANE_INTERSECTION_HPP_INCLUDED
