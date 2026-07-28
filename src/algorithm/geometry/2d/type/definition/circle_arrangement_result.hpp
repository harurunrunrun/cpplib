#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_CIRCLE_ARRANGEMENT_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_CIRCLE_ARRANGEMENT_RESULT_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "point.hpp"

struct CircleArrangementArc{
    std::size_t circle;
    std::size_t first_vertex;
    std::size_t second_vertex;
    long double first_angle;
    long double second_angle;
    bool full_circle;
};

struct CircleArrangementResult{
    std::vector<Point> vertices;
    std::vector<CircleArrangementArc> arcs;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_CIRCLE_ARRANGEMENT_RESULT_HPP_INCLUDED
