#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_TRIANGLE_CONTAINS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_TRIANGLE_CONTAINS_HPP_INCLUDED

#include "../detail/advanced_convex_geometry_detail.hpp"
#include "on_segment.hpp"
#include "side_of_directed_line.hpp"

inline bool triangle_contains(
    const Point& first,
    const Point& second,
    const Point& third,
    const Point& point
){
    if(on_segment({first, second}, point) ||
        on_segment({second, third}, point) ||
        on_segment({third, first}, point)){
        return true;
    }
    if(advanced_geometry_detail::cross_sign(
        second - first, third - first
    ) == 0){
        return false;
    }
    const auto resolved_side = [&point](
        const Point& edge_first,
        const Point& edge_second
    ){
        const Point direction = edge_second - edge_first;
        const long double value = cross(direction, point - edge_first);
        if(geometry_sign(abs(direction)) != 0){
            const int side = side_of_directed_line(
                {edge_first, edge_second}, point
            );
            if(side != 0) return side;
        }
        return (value > 0.0L) - (value < 0.0L);
    };
    const int first_side = resolved_side(first, second);
    const int second_side = resolved_side(second, third);
    const int third_side = resolved_side(third, first);
    const bool nonnegative =
        first_side >= 0 && second_side >= 0 && third_side >= 0;
    const bool nonpositive =
        first_side <= 0 && second_side <= 0 && third_side <= 0;
    return nonnegative || nonpositive;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_TRIANGLE_CONTAINS_HPP_INCLUDED
