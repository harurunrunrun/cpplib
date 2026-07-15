#pragma once

#include <array>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "convex_polyhedron3.hpp"

inline std::vector<std::array<std::size_t, 3>>
convex_polyhedron_face_adjacency(const ConvexPolyhedron3& polyhedron){
    const std::size_t sentinel = polyhedron.faces.size();
    std::vector<std::array<std::size_t, 3>> result(
        polyhedron.faces.size(), {sentinel, sentinel, sentinel}
    );
    using Incident = std::pair<std::size_t, std::size_t>;
    std::map<std::array<std::size_t, 2>, std::vector<Incident>> incidents;
    for(std::size_t face_index = 0; face_index < polyhedron.faces.size(); ++face_index){
        const auto& face = polyhedron.faces[face_index];
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        for(std::size_t edge_index = 0; edge_index < 3; ++edge_index){
            std::array<std::size_t, 2> edge{
                face[edge_index], face[(edge_index + 1) % 3]
            };
            if(edge[1] < edge[0]) std::swap(edge[0], edge[1]);
            incidents[edge].push_back({face_index, edge_index});
        }
    }
    for(const auto& [edge, adjacent]: incidents){
        static_cast<void>(edge);
        if(adjacent.size() > 2)[[unlikely]]{
            throw std::domain_error("non-manifold convex polyhedron edge");
        }
        if(adjacent.size() == 2){
            result[adjacent[0].first][adjacent[0].second] = adjacent[1].first;
            result[adjacent[1].first][adjacent[1].second] = adjacent[0].first;
        }
    }
    return result;
}
