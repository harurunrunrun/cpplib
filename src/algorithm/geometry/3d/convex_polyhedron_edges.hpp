#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "adaptive_orient3d.hpp"
#include "convex_polyhedron3.hpp"

inline std::vector<std::array<std::size_t, 2>> convex_polyhedron_edges(
    const ConvexPolyhedron3& polyhedron
){
    std::map<std::array<std::size_t, 2>, std::vector<std::size_t>> opposites;
    for(const auto& face: polyhedron.faces){
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        for(std::size_t index = 0; index < 3; ++index){
            std::array<std::size_t, 2> edge{
                face[index], face[(index + 1) % 3]
            };
            if(edge[1] < edge[0]) std::swap(edge[0], edge[1]);
            opposites[edge].push_back(face[(index + 2) % 3]);
        }
    }
    std::vector<std::array<std::size_t, 2>> edges;
    edges.reserve(opposites.size());
    for(const auto& [edge, adjacent]: opposites){
        if(adjacent.size() > 2)[[unlikely]]{
            throw std::domain_error("non-manifold convex polyhedron edge");
        }
        if(adjacent.size() == 2 and adaptive_orient3d(
            polyhedron.vertices[edge[0]], polyhedron.vertices[edge[1]],
            polyhedron.vertices[adjacent[0]], polyhedron.vertices[adjacent[1]]
        ) == 0) continue;
        edges.push_back(edge);
    }
    return edges;
}
