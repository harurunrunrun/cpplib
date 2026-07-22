#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERICAL_POLYGON_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERICAL_POLYGON_AREA_HPP_INCLUDED

#include <cmath>
#include <stdexcept>
#include <vector>

#include "spherical_triangle_area.hpp"

inline long double spherical_polygon_area(
    const std::vector<Point3>& polygon,
    long double radius = 1.0L
){
    geometry3d_spherical_detail::validate_radius(radius);
    if(polygon.size() < 3) return 0.0L;
    std::vector<Point3> unit;
    unit.reserve(polygon.size());
    for(const Point3& point: polygon){
        unit.push_back(geometry3d_spherical_detail::unit_direction(
            point, "invalid spherical polygon vertex"
        ));
    }
    long double signed_area = 0.0L;
    for(std::size_t index = 1; index + 1 < unit.size(); ++index){
        signed_area += geometry3d_spherical_detail::signed_triangle_area_unit(
            unit[0], unit[index], unit[index + 1]
        );
    }
    const long double result = std::abs(signed_area) * radius * radius;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("spherical polygon area overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERICAL_POLYGON_AREA_HPP_INCLUDED
