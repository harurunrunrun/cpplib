#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_POLYGON3_PLANE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_POLYGON3_PLANE_INTERSECTION_HPP_INCLUDED

#include <optional>

#include "../point/cross.hpp"
#include "../predicate/is_zero_vector.hpp"
#include "../predicate/on_plane.hpp"
#include "../shape/plane_plane_intersection.hpp"
#include "polygon3_line_intersection.hpp"

struct Polygon3PlaneIntersection3{
    std::optional<Polygon3> coplanar_polygon;
    std::vector<Point3> points;
    std::vector<Segment3> segments;

    bool empty() const noexcept{
        return !coplanar_polygon && points.empty() && segments.empty();
    }
};

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_POLYGON3_PLANE_INTERSECTION_HPP_INCLUDED
