#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_POINT_POINT_POINT_MINIMUM_ENCLOSING_SPHERE_THREE_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_POINT_POINT_POINT_MINIMUM_ENCLOSING_SPHERE_THREE_POINTS_HPP_INCLUDED

#include <array>

#include "../../detail/point/minimum_enclosing_sphere_detail.hpp"

inline Sphere3 minimum_enclosing_sphere(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    const std::array<Point3, 3> points{{first, second, third}};
    minimum_enclosing_sphere_detail::validate_points(
        points.data(), points.size()
    );
    return minimum_enclosing_sphere_detail::support_sphere(
        points.data(), points.size()
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_POINT_POINT_POINT_MINIMUM_ENCLOSING_SPHERE_THREE_POINTS_HPP_INCLUDED
