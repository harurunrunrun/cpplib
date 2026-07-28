#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_POINT_POINT_POINT_BARYCENTRIC_COORDINATES_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_POINT_POINT_POINT_BARYCENTRIC_COORDINATES_DETAIL_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>

#include "../../aggregate/all/is_finite.hpp"
#include "../../type/definition/point3.hpp"

namespace geometry3d_barycentric_detail{

template<std::size_t Size>
inline long double edge_scale(const std::array<Point3, Size>& points){
    long double result = 0.0L;
    for(const Point3& current: points){
        if(!geometry3d_is_finite(current))[[unlikely]]{
            throw std::invalid_argument(
                "barycentric coordinates require finite points"
            );
        }
    }
    for(std::size_t index = 1; index < points.size(); ++index){
        const Geometry3DNormalizedDifference difference =
            geometry3d_normalized_difference(points[index], points.front());
        if(difference.value.x != 0.0L || difference.value.y != 0.0L
            || difference.value.z != 0.0L){
            result = std::max(result, difference.scale);
        }
    }
    return result;
}

inline Point3 normalized_difference(
    const Point3& left,
    const Point3& right,
    long double scale
){
    return geometry3d_safe_normalized_difference(left, right, scale);
}

inline long double vector_scale(const Point3& first, const Point3& second){
    return std::max({
        std::abs(first.x), std::abs(first.y), std::abs(first.z),
        std::abs(second.x), std::abs(second.y), std::abs(second.z),
    });
}

inline long double vector_scale(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return std::max({
        vector_scale(first, second),
        std::abs(third.x), std::abs(third.y), std::abs(third.z),
    });
}

}  // namespace geometry3d_barycentric_detail

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_POINT_POINT_POINT_BARYCENTRIC_COORDINATES_DETAIL_HPP_INCLUDED
