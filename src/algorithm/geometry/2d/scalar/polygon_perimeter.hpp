#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POLYGON_PERIMETER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POLYGON_PERIMETER_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "distance.hpp"

inline long double polygon_perimeter(const std::vector<Point>& polygon){
    if(polygon.size() <= 1) return 0.0L;
    long double result = 0.0L;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        result += distance(polygon[index], polygon[(index + 1) % polygon.size()]);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POLYGON_PERIMETER_HPP_INCLUDED
