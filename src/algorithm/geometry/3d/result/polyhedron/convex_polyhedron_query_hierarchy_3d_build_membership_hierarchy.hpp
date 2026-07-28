#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_BUILD_MEMBERSHIP_HIERARCHY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_BUILD_MEMBERSHIP_HIERARCHY_HPP_INCLUDED

#include "../../detail/polyhedron/convex_polyhedron_query_hierarchy_3d_detail.hpp"

inline void ConvexPolyhedronQueryHierarchy3D::build_membership_hierarchy(){
    using namespace convex_polyhedron_query_hierarchy_3d_detail;
    vertex_sum_ = {};
    for(const Point3& point: polyhedron_.vertices){
        const ExactVector3 exact = exact_point(point);
        for(std::size_t axis = 0; axis < 3; ++axis){
            vertex_sum_[axis] = geometry3d_adaptive_detail::add(
                vertex_sum_[axis], exact[axis]
            );
        }
    }
    vertex_count_factor_ = geometry3d_adaptive_detail::exact_dyadic(
        static_cast<long double>(polyhedron_.vertices.size())
    );

    std::vector<ExactVector3> directions;
    directions.reserve(polyhedron_.vertices.size());
    for(const Point3& point: polyhedron_.vertices){
        directions.push_back(centered_direction(point));
    }
    std::vector<std::size_t> labels(polyhedron_.faces.size());
    std::iota(labels.begin(), labels.end(), std::size_t{0});
    membership_hierarchy_ = RadialHierarchy(
        std::move(directions), polyhedron_.faces, labels
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_BUILD_MEMBERSHIP_HIERARCHY_HPP_INCLUDED
