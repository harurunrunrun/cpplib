#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_CONVEX_POLYHEDRON_EDGES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_CONVEX_POLYHEDRON_EDGES_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../integer/point_point_point_point/adaptive_orient3d.hpp"
#include "polyhedron_edges.hpp"
#include "../../type/definition/convex_polyhedron3.hpp"

inline std::vector<std::array<std::size_t, 2>> convex_polyhedron_edges(
    const ConvexPolyhedron3& polyhedron
){
    using polyhedron_edges_detail::EdgeUse;
    std::vector<EdgeUse> uses;
    uses.reserve(polyhedron.faces.size() * 3);
    for(const auto& face: polyhedron.faces){
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        for(std::size_t index = 0; index < 3; ++index){
            const std::size_t first = face[index];
            const std::size_t second = face[(index + 1) % 3];
            uses.push_back({
                std::min(first, second), std::max(first, second),
                face[(index + 2) % 3],
            });
        }
    }
    // 安定な基数ソートで辺の辞書順に並べる（同じ辺の中では面の順を保つ）。
    polyhedron_edges_detail::radix_sort(uses, polyhedron.vertices.size());
    std::vector<std::array<std::size_t, 2>> edges;
    for(std::size_t begin = 0; begin < uses.size();){
        std::size_t end = begin + 1;
        while(end < uses.size() && uses[end].lower == uses[begin].lower
            && uses[end].upper == uses[begin].upper){
            ++end;
        }
        if(end - begin > 2)[[unlikely]]{
            throw std::domain_error("non-manifold convex polyhedron edge");
        }
        const bool coplanar_diagonal = end - begin == 2
            && adaptive_orient3d(
                polyhedron.vertices[uses[begin].lower],
                polyhedron.vertices[uses[begin].upper],
                polyhedron.vertices[uses[begin].opposite],
                polyhedron.vertices[uses[begin + 1].opposite]
            ) == 0;
        if(!coplanar_diagonal){
            edges.push_back({uses[begin].lower, uses[begin].upper});
        }
        begin = end;
    }
    return edges;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_CONVEX_POLYHEDRON_EDGES_HPP_INCLUDED
