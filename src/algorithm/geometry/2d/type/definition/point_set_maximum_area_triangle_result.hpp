#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_POINT_SET_MAXIMUM_AREA_TRIANGLE_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_POINT_SET_MAXIMUM_AREA_TRIANGLE_RESULT_HPP_INCLUDED

#include <array>

#include "point.hpp"

struct PointSetMaximumAreaTriangleResult{
    std::array<Point, 3> vertices{};
    long double doubled_area = 0.0L;
    bool valid = false;

    long double area() const noexcept{ return doubled_area / 2.0L; }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_POINT_SET_MAXIMUM_AREA_TRIANGLE_RESULT_HPP_INCLUDED
