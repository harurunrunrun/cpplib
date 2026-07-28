#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_POINT_POINT_POINT_CIRCUMCIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_POINT_POINT_POINT_CIRCUMCIRCLE_HPP_INCLUDED

#include "../../detail/segment_point_set/advanced_convex_geometry_detail.hpp"
#include "../../point/point_point_point/circumcenter.hpp"

inline Circle circumcircle(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point center = circumcenter(first, second, third);
    return {center, advanced_geometry_detail::length(center - first)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_POINT_POINT_POINT_CIRCUMCIRCLE_HPP_INCLUDED
