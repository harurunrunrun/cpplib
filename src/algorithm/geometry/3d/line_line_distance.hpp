#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_DISTANCE_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "cross.hpp"
#include "dot.hpp"
#include "line3.hpp"
#include "linear_closest_detail.hpp"
#include "unit.hpp"

inline long double distance(const Line3& first, const Line3& second){
    using namespace geometry3d_linear_closest_detail;
    const Geometry3DNormalizedDifference first_difference =
        geometry3d_normalized_difference(first.b, first.a);
    const Geometry3DNormalizedDifference second_difference =
        geometry3d_normalized_difference(second.b, second.a);
    if(!nonzero(first_difference) || !nonzero(second_difference))
        [[unlikely]]{
        throw std::invalid_argument("degenerate 3D linear primitive");
    }
    const Point3 first_direction = unit(first_difference.value);
    const Point3 second_direction = unit(second_difference.value);
    const Geometry3DNormalizedDifference origin_difference =
        geometry3d_normalized_difference(first.a, second.a);
    const Point3 product = cross(first_direction, second_direction);
    const long double sine = std::hypot(
        product.x, product.y, product.z
    );
    long double normalized_distance;
    if(sine == 0.0L){
        const Point3 perpendicular = cross(
            origin_difference.value, first_direction
        );
        normalized_distance = std::hypot(
            perpendicular.x, perpendicular.y, perpendicular.z
        );
    }else{
        normalized_distance = std::abs(dot(
            origin_difference.value, product / sine
        ));
    }
    return checked_scale_product(
        normalized_distance,
        origin_difference.scale,
        "3D line distance is not representable"
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_DISTANCE_HPP_INCLUDED
