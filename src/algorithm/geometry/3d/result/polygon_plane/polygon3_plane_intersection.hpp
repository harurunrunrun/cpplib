#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYGON_PLANE_POLYGON3_PLANE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYGON_PLANE_POLYGON3_PLANE_INTERSECTION_HPP_INCLUDED

#include <optional>
#include <stdexcept>
#include <utility>

#include "../../point/point_point/cross.hpp"
#include "../../predicate/point/is_zero_vector.hpp"
#include "../../predicate/plane_point/on_plane.hpp"
#include "../../line/plane_plane/plane_plane_intersection.hpp"
#include "../../predicate/polygon/polygon3_coplanar.hpp"
#include "../polygon_line/polygon3_line_intersection.hpp"
#include "../../point/polygon/polygon3_normal.hpp"
#include "../../type/definition/polygon3_plane_intersection3.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYGON_PLANE_POLYGON3_PLANE_INTERSECTION_HPP_INCLUDED
