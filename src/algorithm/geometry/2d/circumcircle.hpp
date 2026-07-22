#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCUMCIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCUMCIRCLE_HPP_INCLUDED

#include "advanced/detail.hpp"
#include "circumcenter.hpp"

inline Circle circumcircle(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point center = circumcenter(first, second, third);
    return {center, advanced_geometry_detail::length(center - first)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCUMCIRCLE_HPP_INCLUDED
