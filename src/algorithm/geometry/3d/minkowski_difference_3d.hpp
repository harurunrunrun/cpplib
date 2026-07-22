#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_DIFFERENCE_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_DIFFERENCE_3D_HPP_INCLUDED

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "convex_hull_3d.hpp"
#include "convex_polyhedron3.hpp"

namespace minkowski_difference_3d_detail{

inline std::vector<Point3> candidate_points(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.vertices.empty() || second.vertices.empty())[[unlikely]]{
        throw std::invalid_argument(
            "Minkowski difference requires nonempty operands"
        );
    }

    std::vector<Point3> points;
    if(second.vertices.size()
        > points.max_size() / first.vertices.size())[[unlikely]]{
        throw std::length_error(
            "Minkowski difference candidate count exceeds the vector limit"
        );
    }
    points.reserve(first.vertices.size() * second.vertices.size());
    for(const Point3& left: first.vertices){
        for(const Point3& right: second.vertices){
            points.push_back(left - right);
        }
    }
    return points;
}

}  // namespace minkowski_difference_3d_detail

inline ConvexPolyhedron3 minkowski_difference_3d_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    std::uint64_t random_seed
){
    return convex_hull_3d_with_seed(
        minkowski_difference_3d_detail::candidate_points(first, second),
        random_seed
    );
}

inline ConvexPolyhedron3 minkowski_difference_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    return convex_hull_3d(
        minkowski_difference_3d_detail::candidate_points(first, second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINKOWSKI_DIFFERENCE_3D_HPP_INCLUDED
