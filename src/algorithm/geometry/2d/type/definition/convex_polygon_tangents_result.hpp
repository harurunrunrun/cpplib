#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_CONVEX_POLYGON_TANGENTS_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_CONVEX_POLYGON_TANGENTS_RESULT_HPP_INCLUDED

#include <cstddef>
#include <limits>

struct ConvexPolygonTangentResult{
    std::size_t left = std::numeric_limits<std::size_t>::max();
    std::size_t right = std::numeric_limits<std::size_t>::max();
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_CONVEX_POLYGON_TANGENTS_RESULT_HPP_INCLUDED
