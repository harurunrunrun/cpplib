#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DEDUPLICATE_POINTS_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DEDUPLICATE_POINTS_3D_DEFAULT_HPP_INCLUDED

#include "deduplicate_points_3d_detail.hpp"

inline std::vector<Point3> deduplicate_points_3d(
    const std::vector<Point3>& points,
    long double relative_tolerance = 0.0L,
    long double absolute_tolerance = GEOMETRY3D_EPS
){
    if(!geometry3d_is_finite(relative_tolerance)
        || !geometry3d_is_finite(absolute_tolerance)
        || relative_tolerance < 0 || absolute_tolerance < 0)[[unlikely]]{
        throw std::invalid_argument("invalid 3D point deduplication tolerance");
    }
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite point in 3D point set");
        }
    }
    if(relative_tolerance != 0.0L){
        return deduplicate_points_3d_detail::quadratic_deduplicate(
            points, relative_tolerance, absolute_tolerance
        );
    }
    if(absolute_tolerance == 0.0L){
        return deduplicate_points_3d_detail::exact_deduplicate(points);
    }
    return deduplicate_points_3d_detail::absolute_deduplicate(
        points, absolute_tolerance
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DEDUPLICATE_POINTS_3D_DEFAULT_HPP_INCLUDED
