#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE_LINE_LINE_SKEW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE_LINE_LINE_SKEW_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../result/line_line/line_line_intersection.hpp"
#include "line_line_parallel.hpp"

inline bool skew(const Line3& first, const Line3& second){
    return !parallel(first, second) &&
        std::holds_alternative<std::monostate>(
            line_line_intersection(first, second)
        );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE_LINE_LINE_SKEW_HPP_INCLUDED
