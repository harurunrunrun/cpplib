#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_CENTERED_DIRECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_CENTERED_DIRECTION_HPP_INCLUDED

#include "../../detail/polyhedron/convex_polyhedron_query_hierarchy_3d_detail.hpp"

inline convex_polyhedron_query_hierarchy_3d_detail::ExactVector3
ConvexPolyhedronQueryHierarchy3D::centered_direction(
    const Point3& point
) const{
    using namespace convex_polyhedron_query_hierarchy_3d_detail;
    const ExactVector3 exact = exact_point(point);
    ExactVector3 result{};
    for(std::size_t axis = 0; axis < 3; ++axis){
        result[axis] = geometry3d_adaptive_detail::subtract(
            geometry3d_adaptive_detail::multiply(
                vertex_count_factor_, exact[axis]
            ),
            vertex_sum_[axis]
        );
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_CENTERED_DIRECTION_HPP_INCLUDED
