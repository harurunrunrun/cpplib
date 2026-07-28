#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RESULT_LINE_LINE_LINE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RESULT_LINE_LINE_LINE_INTERSECTION_HPP_INCLUDED

#include <optional>

#include "../../scalar/vector_vector/vector_products.hpp"
#include "../../vector/line/direction_line.hpp"

namespace integer_geometry{

inline std::optional<Point> line_intersection(
    const Line& first,
    const Line& second
){
    const Vector first_direction = direction(first);
    const Vector second_direction = direction(second);
    const Rational denominator = cross(first_direction, second_direction);
    if(denominator.is_zero()) return std::nullopt;
    const Rational parameter =
        cross(second.a - first.a, second_direction) / denominator;
    return first.a + first_direction * parameter;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RESULT_LINE_LINE_LINE_INTERSECTION_HPP_INCLUDED
