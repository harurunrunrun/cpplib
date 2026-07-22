#ifndef CPPLIB_TEST_STANDALONE_GEOMETRY_3D_COLLISION_TEST_COMMON_HPP_INCLUDED
#define CPPLIB_TEST_STANDALONE_GEOMETRY_3D_COLLISION_TEST_COMMON_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/shape/convex_hull_3d.hpp"
#include "../../src/algorithm/geometry/3d/scalar/dot.hpp"
#include "geometry_3d_api_test_common.hpp"

inline ConvexPolyhedron3 geometry3d_collision_box(
    const Point3& center,
    const Point3& half_extent
){
    std::vector<Point3> points;
    for(int x: {-1, 1}) for(int y: {-1, 1}) for(int z: {-1, 1}){
        points.push_back({
            center.x + static_cast<long double>(x) * half_extent.x,
            center.y + static_cast<long double>(y) * half_extent.y,
            center.z + static_cast<long double>(z) * half_extent.z
        });
    }
    return convex_hull_3d(std::move(points));
}

inline ConvexPolyhedron3 geometry3d_collision_axis_box(
    const Point3& minimum,
    const Point3& maximum
){
    ConvexPolyhedron3 result;
    result.affine_dimension = 3;
    result.vertices = {
        {minimum.x, minimum.y, minimum.z},
        {minimum.x, minimum.y, maximum.z},
        {minimum.x, maximum.y, minimum.z},
        {minimum.x, maximum.y, maximum.z},
        {maximum.x, minimum.y, minimum.z},
        {maximum.x, minimum.y, maximum.z},
        {maximum.x, maximum.y, minimum.z},
        {maximum.x, maximum.y, maximum.z}
    };
    result.faces = {
        {0, 3, 2}, {0, 1, 3}, {4, 6, 7}, {4, 7, 5},
        {0, 4, 5}, {0, 5, 1}, {2, 3, 7}, {2, 7, 6},
        {0, 2, 6}, {0, 6, 4}, {1, 5, 7}, {1, 7, 3}
    };
    return result;
}

inline bool geometry3d_collision_is_unit_finite(const Point3& point){
    const long double length = std::hypot(point.x, point.y, point.z);
    return std::isfinite(length)
        && geometry3d_api_close(length, 1.0L, 1.0e-9L);
}

inline ConvexPolyhedron3 geometry3d_collision_tetra(Point3 shift = {}){
    return convex_hull_3d({
        shift + Point3{0, 0, 0}, shift + Point3{2, 0, 0},
        shift + Point3{0, 2, 0}, shift + Point3{0, 0, 2}
    });
}

inline ConvexPolyhedron3 geometry3d_collision_rotated_z_box(
    const Point3& center,
    const Point3& half_extent,
    long double angle
){
    const long double cosine = std::cos(angle);
    const long double sine = std::sin(angle);
    std::vector<Point3> points;
    for(int x: {-1, 1}) for(int y: {-1, 1}) for(int z: {-1, 1}){
        const long double local_x = static_cast<long double>(x) * half_extent.x;
        const long double local_y = static_cast<long double>(y) * half_extent.y;
        points.push_back(center + Point3{
            cosine * local_x - sine * local_y,
            sine * local_x + cosine * local_y,
            static_cast<long double>(z) * half_extent.z
        });
    }
    return convex_hull_3d(std::move(points));
}

inline long double geometry3d_box_distance(
    const Point3& first_center,
    const Point3& first_half,
    const Point3& second_center,
    const Point3& second_half
){
    const long double dx = std::max(
        0.0L, std::abs(first_center.x - second_center.x)
            - first_half.x - second_half.x
    );
    const long double dy = std::max(
        0.0L, std::abs(first_center.y - second_center.y)
            - first_half.y - second_half.y
    );
    const long double dz = std::max(
        0.0L, std::abs(first_center.z - second_center.z)
            - first_half.z - second_half.z
    );
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

inline Point3 geometry3d_random_point(
    std::mt19937_64& random,
    long double minimum,
    long double maximum
){
    std::uniform_real_distribution<long double> distribution(minimum, maximum);
    return {distribution(random), distribution(random), distribution(random)};
}

inline bool geometry3d_inside_box(
    const Point3& point,
    const Point3& center,
    const Point3& half,
    long double tolerance = 1.0e-8L
){
    return std::abs(point.x - center.x) <= half.x + tolerance
        && std::abs(point.y - center.y) <= half.y + tolerance
        && std::abs(point.z - center.z) <= half.z + tolerance;
}

#endif  // CPPLIB_TEST_STANDALONE_GEOMETRY_3D_COLLISION_TEST_COMMON_HPP_INCLUDED
