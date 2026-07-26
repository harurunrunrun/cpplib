#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ORTHOCENTER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ORTHOCENTER_HPP_INCLUDED

#include "circumcenter.hpp"

inline Point orthocenter(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point center = circumcenter(first, second, third);
    return first + (second - center) + (third - center);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ORTHOCENTER_HPP_INCLUDED
