#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINTS_FROM_TWO_DISTANCES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINTS_FROM_TWO_DISTANCES_HPP_INCLUDED

#include <cmath>
#include <stdexcept>
#include <vector>

#include "circle_circle_cross_points.hpp"

inline std::vector<Point> points_from_two_distances(
    const Point& first,
    long double first_distance,
    const Point& second,
    long double second_distance
){
    if(!std::isfinite(first_distance) || !std::isfinite(second_distance) ||
       first_distance < 0.0L || second_distance < 0.0L){
        throw std::invalid_argument("distances must be finite and nonnegative");
    }
    if(first == second){
        if(geometry_sign(first_distance - second_distance) != 0) return {};
        if(geometry_sign(first_distance) == 0) return {first};
        throw std::domain_error("the two positive-distance loci coincide");
    }
    return circle_circle_cross_points(
        Circle{first, first_distance},
        Circle{second, second_distance}
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINTS_FROM_TWO_DISTANCES_HPP_INCLUDED
