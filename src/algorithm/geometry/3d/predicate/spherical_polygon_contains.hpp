#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SPHERICAL_POLYGON_CONTAINS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SPHERICAL_POLYGON_CONTAINS_HPP_INCLUDED

#include <cmath>
#include <stdexcept>
#include <vector>

#include "../scalar/scalar_triple.hpp"
#include "../scalar/spherical_central_angle.hpp"

namespace geometry3d_spherical_detail{

inline bool on_minor_arc(
    const Point3& first,
    const Point3& second,
    const Point3& query
){
    if(antipodal(first, second))[[unlikely]]{
        throw std::domain_error("antipodal spherical polygon edge");
    }
    const long double whole = spherical_central_angle(first, second);
    const long double parts = spherical_central_angle(first, query)
        + spherical_central_angle(query, second);
    return std::abs(parts - whole) <= 256.0L
        * std::numeric_limits<long double>::epsilon()
        * std::max(1.0L, whole);
}

}  // namespace geometry3d_spherical_detail

inline bool spherical_polygon_contains(
    const std::vector<Point3>& polygon,
    const Point3& query
){
    if(polygon.size() < 3) return false;
    std::vector<Point3> unit;
    unit.reserve(polygon.size());
    for(const Point3& point: polygon){
        unit.push_back(geometry3d_spherical_detail::unit_direction(
            point, "invalid spherical polygon vertex"
        ));
    }
    long double signed_area = 0.0L;
    for(std::size_t index = 1; index + 1 < unit.size(); ++index){
        const long double numerator = scalar_triple(
            unit[0], unit[index], unit[index + 1]
        );
        const long double denominator = 1.0L + dot(unit[0], unit[index])
            + dot(unit[index], unit[index + 1])
            + dot(unit[index + 1], unit[0]);
        if(numerator == 0.0L and denominator == 0.0L)[[unlikely]]{
            throw std::domain_error("ambiguous spherical polygon");
        }
        signed_area += 2.0L * std::atan2(numerator, denominator);
    }
    const Point3 direction = geometry3d_spherical_detail::unit_direction(
        query, "invalid spherical polygon query"
    );
    for(std::size_t index = 0; index < unit.size(); ++index){
        const Point3& first = unit[index];
        const Point3& second = unit[(index + 1) % unit.size()];
        if(geometry3d_spherical_detail::on_minor_arc(first, second, direction)){
            return true;
        }
    }
    if(std::abs(signed_area) <= 256.0L
        * std::numeric_limits<long double>::epsilon()) return false;
    long double winding = 0.0L;
    for(std::size_t index = 0; index < unit.size(); ++index){
        const Point3& first = unit[index];
        const Point3& second = unit[(index + 1) % unit.size()];
        if(geometry3d_spherical_detail::antipodal(direction, first)
            || geometry3d_spherical_detail::antipodal(direction, second)){
            return false;
        }
        const Point3 first_tangent = first - dot(direction, first) * direction;
        const Point3 second_tangent = second - dot(direction, second) * direction;
        const Point3 first_unit = geometry3d_spherical_detail::unit_direction(
            first_tangent, "singular spherical polygon winding"
        );
        const Point3 second_unit = geometry3d_spherical_detail::unit_direction(
            second_tangent, "singular spherical polygon winding"
        );
        winding += std::atan2(
            dot(direction, cross(first_unit, second_unit)),
            std::clamp(dot(first_unit, second_unit), -1.0L, 1.0L)
        );
    }
    return std::abs(winding) > GEOMETRY3D_PI
        and winding * signed_area > 0.0L;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SPHERICAL_POLYGON_CONTAINS_HPP_INCLUDED
