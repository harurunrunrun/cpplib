#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_DIAMETER_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_DIAMETER_RESULT_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <limits>

struct ConvexPolygonDiameterResult{
    std::size_t first = std::numeric_limits<std::size_t>::max();
    std::size_t second = std::numeric_limits<std::size_t>::max();
    long double squared_distance = 0.0L;

    long double distance() const{
        return std::sqrt(squared_distance);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_DIAMETER_RESULT_HPP_INCLUDED
