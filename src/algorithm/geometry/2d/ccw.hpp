#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CCW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CCW_HPP_INCLUDED

#include "cross.hpp"
#include "dot.hpp"
#include "abs.hpp"

enum CcwResult{
    CLOCKWISE = -1,
    COUNTER_CLOCKWISE = 1,
    ONLINE_BACK = 2,
    ONLINE_FRONT = -2,
    ON_SEGMENT = 0,
};

inline int ccw(Point first, Point second, Point third){
    second -= first;
    third -= first;
    const long double length = abs(second);
    if(geometry_sign(length) == 0){
        return geometry_sign(abs(third)) == 0 ? ON_SEGMENT : ONLINE_FRONT;
    }
    const long double cross_roundoff =
        std::abs(second.x * third.y) + std::abs(second.y * third.x);
    const int product_sign = geometry_scaled_sign(
        cross(second, third), length, cross_roundoff
    );
    if(product_sign > 0) return COUNTER_CLOCKWISE;
    if(product_sign < 0) return CLOCKWISE;
    const long double projection = dot(second, third);
    const long double squared_length = dot(second, second);
    const long double dot_roundoff =
        std::abs(second.x * third.x) + std::abs(second.y * third.y);
    if(geometry_scaled_sign(projection, length, dot_roundoff) < 0){
        return ONLINE_BACK;
    }
    if(geometry_scaled_sign(
        projection - squared_length,
        length,
        dot_roundoff + squared_length
    ) > 0) return ONLINE_FRONT;
    return ON_SEGMENT;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CCW_HPP_INCLUDED
