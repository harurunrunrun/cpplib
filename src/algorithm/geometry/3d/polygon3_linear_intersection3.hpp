#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_LINEAR_INTERSECTION3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_LINEAR_INTERSECTION3_HPP_INCLUDED

#include <vector>

#include "point3.hpp"
#include "segment3.hpp"

struct Polygon3LinearIntersection3{
    std::vector<Point3> points;
    std::vector<Segment3> segments;

    bool empty() const noexcept{ return points.empty() && segments.empty(); }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_LINEAR_INTERSECTION3_HPP_INCLUDED
