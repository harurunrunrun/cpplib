#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>
#include <vector>

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "plane_coordinate_system.hpp"
#include "planar_intersection3.hpp"
#include "signed_distance.hpp"
#include "tetrahedron_orientation.hpp"

inline PlanarIntersection3 tetrahedron_plane_intersection(
    const Tetrahedron3& tetrahedron,
    const Plane3& plane
){
    if(tetrahedron_orientation(tetrahedron) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const std::array<Point3, 4> vertices{
        tetrahedron.a, tetrahedron.b, tetrahedron.c, tetrahedron.d
    };
    const std::array<std::pair<std::size_t, std::size_t>, 6> edges{{
        {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3},
    }};
    std::array<long double, 4> distances{};
    std::array<int, 4> signs{};
    for(std::size_t index = 0; index < 4; ++index){
        distances[index] = signed_distance(plane, vertices[index]);
        signs[index] = geometry3d_sign(distances[index]);
    }
    Polygon3 points;
    const auto add = [&](const Point3& point){
        for(const Point3& existing: points) if(existing == point) return;
        points.push_back(point);
    };
    for(std::size_t index = 0; index < 4; ++index){
        if(signs[index] == 0) add(vertices[index]);
    }
    for(const auto [first, second]: edges){
        if(signs[first] * signs[second] < 0){
            const long double parameter = distances[first] /
                (distances[first] - distances[second]);
            add(vertices[first] +
                (vertices[second] - vertices[first]) * parameter);
        }
    }
    if(points.empty()) return std::monostate{};
    if(points.size() == 1) return points.front();
    if(points.size() == 2) return Segment3{points[0], points[1]};

    Point3 center;
    for(const Point3& point: points) center += point;
    center /= static_cast<long double>(points.size());
    const PlaneCoordinateSystem3 coordinates = plane_coordinate_system(plane);
    std::sort(points.begin(), points.end(), [&](const Point3& left, const Point3& right){
        const Point3 left_offset = left - center;
        const Point3 right_offset = right - center;
        const long double left_angle = std::atan2(
            dot(left_offset, coordinates.second_axis),
            dot(left_offset, coordinates.first_axis)
        );
        const long double right_angle = std::atan2(
            dot(right_offset, coordinates.second_axis),
            dot(right_offset, coordinates.first_axis)
        );
        return left_angle < right_angle;
    });
    return points;
}
