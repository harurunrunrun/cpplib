#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_POINT_SET_MINIMUM_ENCLOSING_SPHERE_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_POINT_SET_MINIMUM_ENCLOSING_SPHERE_POINTS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../detail/point/minimum_enclosing_sphere_detail.hpp"
#include "../point_point_point_point/minimum_enclosing_sphere_four_points.hpp"
#include "../point/minimum_enclosing_sphere_one_point.hpp"
#include "../point_point_point/minimum_enclosing_sphere_three_points.hpp"
#include "../point_point/minimum_enclosing_sphere_two_points.hpp"

inline Sphere3 minimum_enclosing_sphere(
    std::vector<Point3> points,
    std::uint64_t seed = 0x9E3779B97F4A7C15ULL
){
    if(points.empty())[[unlikely]]{
        throw std::invalid_argument("minimum enclosing sphere requires a point");
    }
    minimum_enclosing_sphere_detail::validate_points(
        points.data(), points.size()
    );
    std::mt19937_64 random(seed);
    std::shuffle(points.begin(), points.end(), random);

    Sphere3 sphere = minimum_enclosing_sphere(points[0]);
    for(std::size_t first = 1; first < points.size(); ++first){
        if(minimum_enclosing_sphere_detail::contains(
            sphere, points[first]
        )) continue;
        sphere = minimum_enclosing_sphere(points[first]);
        for(std::size_t second = 0; second < first; ++second){
            if(minimum_enclosing_sphere_detail::contains(
                sphere, points[second]
            )){
                continue;
            }
            sphere = minimum_enclosing_sphere(points[first], points[second]);
            for(std::size_t third = 0; third < second; ++third){
                if(minimum_enclosing_sphere_detail::contains(
                    sphere, points[third]
                )){
                    continue;
                }
                sphere = minimum_enclosing_sphere(
                    points[first], points[second], points[third]
                );
                for(std::size_t fourth = 0; fourth < third; ++fourth){
                    if(minimum_enclosing_sphere_detail::contains(
                        sphere, points[fourth]
                    )) continue;
                    sphere = minimum_enclosing_sphere(
                        points[first], points[second],
                        points[third], points[fourth]
                    );
                }
            }
        }
    }
    return sphere;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_POINT_SET_MINIMUM_ENCLOSING_SPHERE_POINTS_HPP_INCLUDED
