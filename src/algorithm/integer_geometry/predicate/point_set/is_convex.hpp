#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_POINT_SET_IS_CONVEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_POINT_SET_IS_CONVEX_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "../../integer/point_point_point/orientation.hpp"

namespace integer_geometry{

inline bool is_convex(const std::vector<Point>& polygon, bool strict = false){
    if(polygon.size() < 3) return false;
    int direction_sign = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const int turn = orientation(
            polygon[index],
            polygon[(index + 1) % polygon.size()],
            polygon[(index + 2) % polygon.size()]
        );
        if(turn == 0){
            if(strict) return false;
            continue;
        }
        if(direction_sign != 0 && direction_sign != turn) return false;
        direction_sign = turn;
    }
    return direction_sign != 0;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_POINT_SET_IS_CONVEX_HPP_INCLUDED
