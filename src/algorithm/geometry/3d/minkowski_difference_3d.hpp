#pragma once

#include <stdexcept>
#include <vector>

#include "convex_hull_3d.hpp"
#include "convex_polyhedron3.hpp"

inline ConvexPolyhedron3 minkowski_difference_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.vertices.empty() || second.vertices.empty())[[unlikely]]{
        throw std::invalid_argument(
            "Minkowski difference requires nonempty operands"
        );
    }
    std::vector<Point3> points;
    points.reserve(first.vertices.size() * second.vertices.size());
    for(const Point3& left: first.vertices){
        for(const Point3& right: second.vertices){
            points.push_back(left - right);
        }
    }
    return convex_hull_3d(std::move(points));
}
