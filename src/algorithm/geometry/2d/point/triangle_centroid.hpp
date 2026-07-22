#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_TRIANGLE_CENTROID_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_TRIANGLE_CENTROID_HPP_INCLUDED

#include "../core/types.hpp"

inline Point triangle_centroid(
    const Point& first,
    const Point& second,
    const Point& third
){
    return first +
        (second - first) / 3.0L +
        (third - first) / 3.0L;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_TRIANGLE_CENTROID_HPP_INCLUDED
