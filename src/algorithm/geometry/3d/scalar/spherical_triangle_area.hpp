#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERICAL_TRIANGLE_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERICAL_TRIANGLE_AREA_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "scalar_triple.hpp"
#include "spherical_central_angle.hpp"

namespace geometry3d_spherical_detail{

inline long double signed_triangle_area_unit(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    const long double numerator = scalar_triple(first, second, third);
    const long double denominator = 1.0L + dot(first, second)
        + dot(second, third) + dot(third, first);
    if(numerator == 0.0L && denominator == 0.0L)[[unlikely]]{
        throw std::domain_error("ambiguous spherical triangle with antipodal boundary");
    }
    return 2.0L * std::atan2(numerator, denominator);
}

}  // namespace geometry3d_spherical_detail

inline long double spherical_triangle_area(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    long double radius = 1.0L
){
    geometry3d_spherical_detail::validate_radius(radius);
    const Point3 a = geometry3d_spherical_detail::unit_direction(
        first, "invalid first spherical triangle vertex"
    );
    const Point3 b = geometry3d_spherical_detail::unit_direction(
        second, "invalid second spherical triangle vertex"
    );
    const Point3 c = geometry3d_spherical_detail::unit_direction(
        third, "invalid third spherical triangle vertex"
    );
    const long double unit_area = std::abs(
        geometry3d_spherical_detail::signed_triangle_area_unit(a, b, c)
    );
    const long double result = unit_area * radius * radius;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("spherical triangle area overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERICAL_TRIANGLE_AREA_HPP_INCLUDED
