#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_CIRCUMCIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_CIRCUMCIRCLE_HPP_INCLUDED

#include "../detail/advanced_convex_geometry_detail.hpp"
#include "../point/circumcenter.hpp"

inline Circle circumcircle(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point center = circumcenter(first, second, third);
    return {center, advanced_geometry_detail::length(center - first)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_CIRCUMCIRCLE_HPP_INCLUDED
