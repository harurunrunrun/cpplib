#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYGON3_PLANE_INTERSECTION3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYGON3_PLANE_INTERSECTION3_HPP_INCLUDED

#include <optional>
#include <vector>

#include "polygon3.hpp"
#include "segment3.hpp"

struct Polygon3PlaneIntersection3{
    std::optional<Polygon3> coplanar_polygon;
    std::vector<Point3> points;
    std::vector<Segment3> segments;

    bool empty() const noexcept{
        return !coplanar_polygon && points.empty() && segments.empty();
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYGON3_PLANE_INTERSECTION3_HPP_INCLUDED
