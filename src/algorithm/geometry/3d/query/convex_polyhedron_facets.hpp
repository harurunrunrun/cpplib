#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_FACETS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_FACETS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <map>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../predicate/adaptive_orient3d.hpp"
#include "../core/convex_polyhedron3.hpp"

struct ConvexPolyhedronFacet3{
    std::vector<std::size_t> boundary;
    std::vector<std::size_t> triangles;
};

namespace convex_polyhedron_facets_detail{

struct DisjointSet{
    std::vector<std::size_t> parent;

    explicit DisjointSet(std::size_t size): parent(size){
        std::iota(parent.begin(), parent.end(), 0);
    }

    std::size_t leader(std::size_t vertex){
        if(parent[vertex] == vertex) return vertex;
        return parent[vertex] = leader(parent[vertex]);
    }

    void merge(std::size_t first, std::size_t second){
        first = leader(first);
        second = leader(second);
        if(first != second) parent[second] = first;
    }
};

struct EdgeUse{
    std::size_t triangle = 0;
    std::size_t opposite = 0;
    std::size_t from = 0;
    std::size_t to = 0;
};

inline std::array<std::size_t, 2> edge_key(
    std::size_t first,
    std::size_t second
){
    if(second < first) std::swap(first, second);
    return {first, second};
}

}  // namespace convex_polyhedron_facets_detail

inline std::vector<ConvexPolyhedronFacet3> convex_polyhedron_facets(
    const ConvexPolyhedron3& polyhedron
){
    using namespace convex_polyhedron_facets_detail;
    if(polyhedron.affine_dimension < 2 || polyhedron.faces.empty()) return {};

    std::map<std::array<std::size_t, 2>, std::vector<EdgeUse>> edge_uses;
    for(std::size_t triangle = 0; triangle < polyhedron.faces.size(); ++triangle){
        const auto& face = polyhedron.faces[triangle];
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        if(face[0] == face[1] || face[1] == face[2] || face[2] == face[0]){
            throw std::domain_error("degenerate convex polyhedron triangle");
        }
        for(std::size_t index = 0; index < 3; ++index){
            const std::size_t from = face[index];
            const std::size_t to = face[(index + 1) % 3];
            edge_uses[edge_key(from, to)].push_back({
                triangle, face[(index + 2) % 3], from, to
            });
        }
    }

    DisjointSet components(polyhedron.faces.size());
    for(const auto& [edge, uses]: edge_uses){
        if(uses.size() > 2)[[unlikely]]{
            throw std::domain_error("non-manifold convex polyhedron edge");
        }
        if(uses.size() != 2) continue;
        if(adaptive_orient3d(
            polyhedron.vertices[edge[0]], polyhedron.vertices[edge[1]],
            polyhedron.vertices[uses[0].opposite],
            polyhedron.vertices[uses[1].opposite]
        ) == 0){
            components.merge(uses[0].triangle, uses[1].triangle);
        }
    }

    std::map<std::size_t, std::vector<std::size_t>> grouped;
    for(std::size_t triangle = 0; triangle < polyhedron.faces.size(); ++triangle){
        grouped[components.leader(triangle)].push_back(triangle);
    }

    std::vector<ConvexPolyhedronFacet3> result;
    result.reserve(grouped.size());
    for(auto& [leader, triangles]: grouped){
        static_cast<void>(leader);
        std::map<std::array<std::size_t, 2>, std::vector<EdgeUse>> local_edges;
        for(std::size_t triangle: triangles){
            const auto& face = polyhedron.faces[triangle];
            for(std::size_t index = 0; index < 3; ++index){
                const std::size_t from = face[index];
                const std::size_t to = face[(index + 1) % 3];
                local_edges[edge_key(from, to)].push_back({
                    triangle, face[(index + 2) % 3], from, to
                });
            }
        }

        std::map<std::size_t, std::size_t> next;
        for(const auto& [edge, uses]: local_edges){
            static_cast<void>(edge);
            if(uses.size() == 2) continue;
            if(uses.size() != 1)[[unlikely]]{
                throw std::domain_error("non-manifold geometric facet");
            }
            if(!next.emplace(uses[0].from, uses[0].to).second)[[unlikely]]{
                throw std::domain_error("inconsistently oriented facet triangles");
            }
        }
        if(next.size() < 3)[[unlikely]]{
            throw std::domain_error("geometric facet has no boundary loop");
        }
        const std::size_t start = next.begin()->first;
        std::vector<std::size_t> boundary;
        boundary.reserve(next.size());
        std::size_t current = start;
        do{
            boundary.push_back(current);
            const auto iterator = next.find(current);
            if(iterator == next.end())[[unlikely]]{
                throw std::domain_error("open geometric facet boundary");
            }
            current = iterator->second;
            if(boundary.size() > next.size())[[unlikely]]{
                throw std::domain_error("self-intersecting geometric facet boundary");
            }
        }while(current != start);
        if(boundary.size() != next.size())[[unlikely]]{
            throw std::domain_error("geometric facet has multiple boundary loops");
        }
        result.push_back({std::move(boundary), std::move(triangles)});
    }
    std::sort(result.begin(), result.end(), [](const auto& left,
                                               const auto& right){
        return left.boundary < right.boundary;
    });
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_FACETS_HPP_INCLUDED
