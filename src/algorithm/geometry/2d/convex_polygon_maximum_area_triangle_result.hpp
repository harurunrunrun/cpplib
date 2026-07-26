#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_MAXIMUM_AREA_TRIANGLE_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_MAXIMUM_AREA_TRIANGLE_RESULT_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>

struct ConvexPolygonMaximumAreaTriangleResult{
    std::array<std::size_t, 3> indices{
        std::numeric_limits<std::size_t>::max(),
        std::numeric_limits<std::size_t>::max(),
        std::numeric_limits<std::size_t>::max(),
    };
    long double doubled_area = 0.0L;

    bool valid() const{
        return indices[0] != std::numeric_limits<std::size_t>::max();
    }

    long double area() const{
        return doubled_area / 2.0L;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_MAXIMUM_AREA_TRIANGLE_RESULT_HPP_INCLUDED
