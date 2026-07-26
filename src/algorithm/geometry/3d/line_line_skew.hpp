#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_SKEW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_SKEW_HPP_INCLUDED

#include <variant>

#include "geometry_primitives.hpp"
#include "line_line_intersection.hpp"
#include "line_line_parallel.hpp"

inline bool skew(const Line3& first, const Line3& second){
    return !parallel(first, second) &&
        std::holds_alternative<std::monostate>(
            line_line_intersection(first, second)
        );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_SKEW_HPP_INCLUDED
