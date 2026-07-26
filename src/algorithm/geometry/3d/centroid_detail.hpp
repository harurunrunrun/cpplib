#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CENTROID_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CENTROID_DETAIL_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

#include "is_finite.hpp"
#include "point3.hpp"

namespace geometry3d_centroid_detail{

template<std::size_t Size>
inline long double average_coordinate(
    const std::array<long double, Size>& coordinates
){
    long double scale = 0.0L;
    for(const long double coordinate: coordinates){
        scale = std::max(scale, std::abs(coordinate));
    }
    if(scale == 0.0L) return 0.0L;
    long double normalized_sum = 0.0L;
    for(const long double coordinate: coordinates){
        normalized_sum += coordinate / scale;
    }
    return (normalized_sum / static_cast<long double>(Size)) * scale;
}

template<std::size_t Size>
inline Point3 centroid(const std::array<Point3, Size>& points){
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("centroid requires finite vertices");
        }
    }
    std::array<long double, Size> x{};
    std::array<long double, Size> y{};
    std::array<long double, Size> z{};
    for(std::size_t index = 0; index < Size; ++index){
        x[index] = points[index].x;
        y[index] = points[index].y;
        z[index] = points[index].z;
    }
    return {
        average_coordinate(x),
        average_coordinate(y),
        average_coordinate(z),
    };
}

}  // namespace geometry3d_centroid_detail

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CENTROID_DETAIL_HPP_INCLUDED
