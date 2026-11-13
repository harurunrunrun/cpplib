#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_CONVEX_POLYHEDRON_FACE_ADJACENCY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_CONVEX_POLYHEDRON_FACE_ADJACENCY_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../type/definition/convex_polyhedron3.hpp"
#include "polyhedron_edges.hpp"

inline std::vector<std::array<std::size_t, 3>>
convex_polyhedron_face_adjacency(const ConvexPolyhedron3& polyhedron){
    const std::size_t sentinel = polyhedron.faces.size();
    std::vector<std::array<std::size_t, 3>> result(
        polyhedron.faces.size(), {sentinel, sentinel, sentinel}
    );
    // opposite に「面の添字 * 3 + 辺の添字」を入れ、安定な基数ソートで
    // 辺の辞書順に並べる（同じ辺の中では面の順を保つ）。
    using polyhedron_edges_detail::EdgeUse;
    std::vector<EdgeUse> uses;
    uses.reserve(polyhedron.faces.size() * 3);
    for(std::size_t face_index = 0; face_index < polyhedron.faces.size(); ++face_index){
        const auto& face = polyhedron.faces[face_index];
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        for(std::size_t edge_index = 0; edge_index < 3; ++edge_index){
            const std::size_t first = face[edge_index];
            const std::size_t second = face[(edge_index + 1) % 3];
            uses.push_back({
                std::min(first, second), std::max(first, second),
                face_index * 3 + edge_index,
            });
        }
    }
    polyhedron_edges_detail::radix_sort(uses, polyhedron.vertices.size());
    for(std::size_t begin = 0; begin < uses.size();){
        std::size_t end = begin + 1;
        while(end < uses.size() && uses[end].lower == uses[begin].lower
            && uses[end].upper == uses[begin].upper){
            ++end;
        }
        if(end - begin > 2)[[unlikely]]{
            throw std::domain_error("non-manifold convex polyhedron edge");
        }
        if(end - begin == 2){
            const std::size_t first = uses[begin].opposite;
            const std::size_t second = uses[begin + 1].opposite;
            result[first / 3][first % 3] = second / 3;
            result[second / 3][second % 3] = first / 3;
        }
        begin = end;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_CONVEX_POLYHEDRON_FACE_ADJACENCY_HPP_INCLUDED
