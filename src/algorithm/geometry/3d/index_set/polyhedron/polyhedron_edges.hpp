#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_POLYHEDRON_EDGES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_POLYHEDRON_EDGES_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../../integer/point_point_point_point/adaptive_orient3d.hpp"
#include "../../type/definition/polyhedron3.hpp"
#include "../../void/polyhedron/polyhedron3_validate.hpp"

namespace polyhedron_edges_detail{

struct EdgeUse{
    std::size_t lower;
    std::size_t upper;
    std::size_t opposite;
};

template<class Key>
inline void counting_sort(
    std::vector<EdgeUse>& edges,
    std::vector<EdgeUse>& buffer,
    std::size_t vertex_count,
    Key key
){
    std::vector<std::size_t> offset(vertex_count + 1);
    for(const EdgeUse& edge: edges) ++offset[key(edge) + 1];
    for(std::size_t index = 1; index < offset.size(); ++index){
        offset[index] += offset[index - 1];
    }
    for(const EdgeUse& edge: edges) buffer[offset[key(edge)]++] = edge;
    edges.swap(buffer);
}

inline void radix_sort(std::vector<EdgeUse>& edges, std::size_t vertex_count){
    std::vector<EdgeUse> buffer(edges.size());
    counting_sort(edges, buffer, vertex_count, [](const EdgeUse& edge){
        return edge.upper;
    });
    counting_sort(edges, buffer, vertex_count, [](const EdgeUse& edge){
        return edge.lower;
    });
}

}  // namespace polyhedron_edges_detail

inline std::vector<std::array<std::size_t, 2>> polyhedron_edges(
    const Polyhedron3& polyhedron
){
    using namespace polyhedron_edges_detail;
    geometry3d_validate(polyhedron);
    std::vector<EdgeUse> uses;
    uses.reserve(polyhedron.faces.size() * 3);
    for(const auto& face: polyhedron.faces){
        for(std::size_t edge = 0; edge < 3; ++edge){
            const std::size_t first = face[edge];
            const std::size_t second = face[(edge + 1) % 3];
            uses.push_back({
                std::min(first, second), std::max(first, second),
                face[(edge + 2) % 3],
            });
        }
    }
    radix_sort(uses, polyhedron.vertices.size());
    std::vector<std::array<std::size_t, 2>> result;
    for(std::size_t begin = 0; begin < uses.size();){
        std::size_t end = begin + 1;
        while(end < uses.size() && uses[end].lower == uses[begin].lower
            && uses[end].upper == uses[begin].upper){
            ++end;
        }
        if(end - begin > 2)[[unlikely]]{
            throw std::domain_error("non-manifold polyhedron edge");
        }
        const bool coplanar_diagonal = end - begin == 2
            && adaptive_orient3d(
                polyhedron.vertices[uses[begin].lower],
                polyhedron.vertices[uses[begin].upper],
                polyhedron.vertices[uses[begin].opposite],
                polyhedron.vertices[uses[begin + 1].opposite]
            ) == 0;
        if(!coplanar_diagonal){
            result.push_back({uses[begin].lower, uses[begin].upper});
        }
        begin = end;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_POLYHEDRON_EDGES_HPP_INCLUDED
