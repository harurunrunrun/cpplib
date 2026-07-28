#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_LINE_ARRANGEMENT_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_LINE_ARRANGEMENT_RESULT_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "point.hpp"

struct LineArrangementEdge{
    std::size_t first_vertex;
    std::size_t second_vertex;
    std::size_t line;
};

struct LineArrangementRay{
    std::size_t vertex;
    Point direction;
    std::size_t line;
};

struct LineArrangementUnboundedLine{
    Point origin;
    Point direction;
    std::size_t line;
};

struct LineArrangementResult{
    std::vector<Point> vertices;
    std::vector<LineArrangementEdge> bounded_edges;
    std::vector<LineArrangementRay> rays;
    std::vector<LineArrangementUnboundedLine> unbounded_lines;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_LINE_ARRANGEMENT_RESULT_HPP_INCLUDED
