#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_MINIMUM_ENCLOSING_SPHERE_WELZL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_MINIMUM_ENCLOSING_SPHERE_WELZL_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "minimum_enclosing_sphere.hpp"

namespace minimum_enclosing_sphere_welzl_detail{

inline Sphere3 boundary_sphere(
    const std::array<Point3, 4>& boundary,
    std::size_t boundary_size
){
    switch(boundary_size){
        case 0: return {{}, -1};
        case 1: return minimum_enclosing_sphere(boundary[0]);
        case 2: return minimum_enclosing_sphere(boundary[0], boundary[1]);
        case 3: return minimum_enclosing_sphere(
            boundary[0], boundary[1], boundary[2]
        );
        default: return minimum_enclosing_sphere(
            boundary[0], boundary[1], boundary[2], boundary[3]
        );
    }
}

inline Sphere3 solve(
    const std::vector<Point3>& points,
    std::size_t point_count,
    std::array<Point3, 4>& boundary,
    std::size_t boundary_size
){
    if(point_count == 0 || boundary_size == boundary.size()){
        return boundary_sphere(boundary, boundary_size);
    }
    const Point3 point = points[point_count - 1];
    Sphere3 sphere = solve(
        points, point_count - 1, boundary, boundary_size
    );
    if(sphere.radius >= 0 &&
       minimum_enclosing_sphere_detail::contains(sphere, point)){
        return sphere;
    }
    boundary[boundary_size] = point;
    return solve(points, point_count - 1, boundary, boundary_size + 1);
}

} // namespace minimum_enclosing_sphere_welzl_detail

inline Sphere3 minimum_enclosing_sphere_welzl(
    std::vector<Point3> points,
    std::uint64_t seed = 0xD1B54A32D192ED03ULL
){
    if(points.empty())[[unlikely]]{
        throw std::invalid_argument("minimum enclosing sphere requires a point");
    }
    minimum_enclosing_sphere_detail::validate_points(points.data(), points.size());
    std::mt19937_64 random(seed);
    std::shuffle(points.begin(), points.end(), random);
    std::array<Point3, 4> boundary{};
    return minimum_enclosing_sphere_welzl_detail::solve(
        points, points.size(), boundary, 0
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_MINIMUM_ENCLOSING_SPHERE_WELZL_HPP_INCLUDED
