#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_LOGARITHMIC_SUPPORT_INDEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_LOGARITHMIC_SUPPORT_INDEX_HPP_INCLUDED

#include "../../detail/polyhedron/convex_polyhedron_query_hierarchy_3d_detail.hpp"

inline std::size_t
ConvexPolyhedronQueryHierarchy3D::logarithmic_support_index(
    const convex_polyhedron_query_hierarchy_3d_detail::ExactVector3& direction
) const{
    using namespace convex_polyhedron_query_hierarchy_3d_detail;
    const RadialLocation location = support_hierarchy_.locate(direction);
    const RadialTriangle& triangle =
        support_hierarchy_.finest_triangle(location.triangle);
    std::size_t zero_count = 0;
    for(const int sign: location.barycentric_signs){
        if(sign < 0){
            throw std::logic_error("negative located polar barycentric weight");
        }
        if(sign == 0) ++zero_count;
    }

    std::size_t answer = no_index;
    if(zero_count >= 2){
        std::size_t corner = no_index;
        for(std::size_t index = 0; index < 3; ++index){
            if(location.barycentric_signs[index] != 0){
                corner = index;
                break;
            }
        }
        if(corner == no_index){
            throw std::logic_error("zero polar support direction");
        }
        answer = support_facet_minimum_vertex_.at(
            triangle.vertices[corner]
        );
    }else if(zero_count == 1){
        std::array<std::size_t, 2> edge{};
        std::size_t count = 0;
        for(std::size_t index = 0; index < 3; ++index){
            if(location.barycentric_signs[index] != 0){
                edge[count++] = triangle.vertices[index];
            }
        }
        if(count != 2){
            throw std::logic_error("invalid polar edge coordinates");
        }
        answer = support_hierarchy_.fine_edge_label(edge[0], edge[1]);
        const auto iterator = support_edge_minimum_vertex_.find(
            radial_edge_key(edge[0], edge[1])
        );
        if(iterator != support_edge_minimum_vertex_.end()){
            answer = std::min(answer, iterator->second);
        }
    }else{
        answer = triangle.label;
    }
    if(answer == no_index || answer >= polyhedron_.vertices.size()){
        throw std::logic_error("polar support label is missing");
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_LOGARITHMIC_SUPPORT_INDEX_HPP_INCLUDED
