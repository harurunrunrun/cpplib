#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_LINE_CUT_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_LINE_CUT_RESULT_HPP_INCLUDED

#include <vector>

#include "types.hpp"

struct PolygonLineCutResult {
    std::vector<std::vector<Point>> left;
    std::vector<std::vector<Point>> right;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_LINE_CUT_RESULT_HPP_INCLUDED
