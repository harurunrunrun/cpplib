#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_PLANE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_PLANE_INTERSECTION_HPP_INCLUDED

#include <optional>
#include <stdexcept>
#include <utility>

#include "cross.hpp"
#include "is_zero_vector.hpp"
#include "on_plane.hpp"
#include "plane_plane_intersection.hpp"
#include "polygon3_coplanar.hpp"
#include "polygon3_line_intersection.hpp"
#include "polygon3_normal.hpp"
#include "polygon3_plane_intersection3.hpp"

inline Polygon3PlaneIntersection3 polygon3_plane_intersection(
    const Polygon3& polygon,
    const Plane3& plane
){
    if(polygon.size() < 3 || !polygon3_coplanar(polygon))[[unlikely]]{
        throw std::invalid_argument("polygon3 plane intersection requires a planar polygon");
    }
    const Plane3 polygon_plane{polygon.front(), polygon3_normal(polygon)};
    if(is_zero_vector(cross(polygon_plane.normal, plane.normal))){
        if(on_plane(plane, polygon.front())) return {polygon, {}, {}};
        return {};
    }
    Polygon3LinearIntersection3 linear = polygon3_line_intersection(
        polygon, plane_plane_intersection(polygon_plane, plane)
    );
    return {
        std::nullopt, std::move(linear.points), std::move(linear.segments)
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_PLANE_INTERSECTION_HPP_INCLUDED
