#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_DETAIL_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../../type/definition/convex_polyhedron3.hpp"
#include "../../point/point_point/cross.hpp"
#include "../../type/definition/plane3.hpp"

namespace convex_polyhedron_intersection_detail{

inline void append_halfspaces(
    const ConvexPolyhedron3& polyhedron,
    std::vector<Plane3>& halfspaces
){
    if(polyhedron.affine_dimension != 3
        || polyhedron.vertices.empty()
        || polyhedron.faces.empty())[[unlikely]]{
        throw std::invalid_argument(
            "halfspace intersection requires a full-dimensional polyhedron"
        );
    }
    halfspaces.reserve(halfspaces.size() + polyhedron.faces.size());
    for(const auto& face: polyhedron.faces){
        for(const std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::invalid_argument(
                    "convex polyhedron face index is out of range"
                );
            }
        }
        const Point3& first = polyhedron.vertices[face[0]];
        const Point3& second = polyhedron.vertices[face[1]];
        const Point3& third = polyhedron.vertices[face[2]];
        halfspaces.push_back({
            first,
            cross(second - first, third - first),
        });
    }
}

inline std::vector<Plane3> combined_halfspaces(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    std::vector<Plane3> halfspaces;
    halfspaces.reserve(first.faces.size() + second.faces.size());
    append_halfspaces(first, halfspaces);
    append_halfspaces(second, halfspaces);
    return halfspaces;
}

}  // namespace convex_polyhedron_intersection_detail

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_DETAIL_HPP_INCLUDED
