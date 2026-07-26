#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_BUILD_SUPPORT_HIERARCHY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_BUILD_SUPPORT_HIERARCHY_HPP_INCLUDED

#include "convex_polyhedron_query_hierarchy_3d_detail.hpp"

inline void ConvexPolyhedronQueryHierarchy3D::build_support_hierarchy(){
    using namespace convex_polyhedron_query_hierarchy_3d_detail;
    const std::vector<ConvexPolyhedronFacet3> facets =
        convex_polyhedron_facets(polyhedron_);
    if(facets.size() < 4){
        throw std::domain_error(
            "three-dimensional convex polyhedron has fewer than four facets"
        );
    }

    std::vector<std::size_t> face_to_facet(
        polyhedron_.faces.size(), no_index
    );
    std::vector<ExactVector3> directions;
    directions.reserve(facets.size());
    support_facet_minimum_vertex_.assign(facets.size(), no_index);
    std::vector<std::vector<std::size_t>> vertex_facets(
        polyhedron_.vertices.size()
    );
    std::map<std::pair<std::size_t, std::size_t>, std::vector<std::size_t>>
        edge_facets;

    for(std::size_t facet = 0; facet < facets.size(); ++facet){
        if(facets[facet].triangles.empty()){
            throw std::logic_error("convex polyhedron facet has no triangle");
        }
        const auto& plane = exact_planes_[facets[facet].triangles.front()];
        directions.push_back({
            plane.coefficient[0],
            plane.coefficient[1],
            plane.coefficient[2],
        });
        for(const std::size_t face: facets[facet].triangles){
            if(face >= face_to_facet.size()
                || face_to_facet[face] != no_index){
                throw std::logic_error("invalid facet triangle partition");
            }
            face_to_facet[face] = facet;
            for(const std::size_t vertex: polyhedron_.faces[face]){
                support_facet_minimum_vertex_[facet] = std::min(
                    support_facet_minimum_vertex_[facet], vertex
                );
                vertex_facets[vertex].push_back(facet);
            }
        }
        if(facets[facet].boundary.size() < 3){
            throw std::domain_error("convex polyhedron facet boundary");
        }
        for(std::size_t edge = 0;
            edge < facets[facet].boundary.size(); ++edge){
            edge_facets[radial_edge_key(
                facets[facet].boundary[edge],
                facets[facet].boundary[
                    (edge + 1) % facets[facet].boundary.size()
                ]
            )].push_back(facet);
        }
    }
    if(std::find(face_to_facet.begin(), face_to_facet.end(), no_index)
        != face_to_facet.end()){
        throw std::logic_error("facet partition misses a triangle");
    }
    for(auto& incident: vertex_facets){
        std::sort(incident.begin(), incident.end());
        incident.erase(
            std::unique(incident.begin(), incident.end()), incident.end()
        );
    }

    std::vector<std::vector<std::pair<std::size_t, std::size_t>>>
        vertex_links(polyhedron_.vertices.size());
    support_edge_minimum_vertex_.clear();
    for(auto& [edge, incident]: edge_facets){
        std::sort(incident.begin(), incident.end());
        incident.erase(
            std::unique(incident.begin(), incident.end()), incident.end()
        );
        if(incident.size() != 2){
            throw std::domain_error(
                "geometric polyhedron edge has invalid facet incidence"
            );
        }
        const auto facet_pair = radial_edge_key(incident[0], incident[1]);
        for(const std::size_t vertex: {edge.first, edge.second}){
            vertex_links[vertex].push_back(facet_pair);
            auto [iterator, inserted] =
                support_edge_minimum_vertex_.try_emplace(
                    facet_pair, vertex
                );
            if(!inserted){
                iterator->second = std::min(iterator->second, vertex);
            }
        }
    }

    std::vector<std::array<std::size_t, 3>> polar_triangles;
    std::vector<std::size_t> polar_labels;
    for(std::size_t vertex = 0;
        vertex < polyhedron_.vertices.size(); ++vertex){
        const auto& incident = vertex_facets[vertex];
        if(incident.size() < 3) continue;

        std::map<std::size_t, std::vector<std::size_t>> adjacency;
        for(const auto& [first, second]: vertex_links[vertex]){
            adjacency[first].push_back(second);
            adjacency[second].push_back(first);
        }
        for(const std::size_t facet: incident){
            auto& adjacent = adjacency[facet];
            std::sort(adjacent.begin(), adjacent.end());
            adjacent.erase(
                std::unique(adjacent.begin(), adjacent.end()), adjacent.end()
            );
            if(adjacent.size() != 2){
                throw std::domain_error(
                    "incident facets do not form a vertex cycle"
                );
            }
        }

        std::vector<std::size_t> cycle;
        cycle.reserve(incident.size());
        const std::size_t start = *std::min_element(
            incident.begin(), incident.end()
        );
        std::size_t previous = no_index;
        std::size_t current = start;
        do{
            cycle.push_back(current);
            const auto& adjacent = adjacency.at(current);
            const std::size_t next =
                adjacent[0] != previous ? adjacent[0] : adjacent[1];
            previous = current;
            current = next;
            if(cycle.size() > incident.size()){
                throw std::domain_error("facet cycle repeats");
            }
        }while(current != start);
        if(cycle.size() != incident.size()){
            throw std::domain_error("facet cycle is disconnected");
        }

        int orientation = 0;
        for(std::size_t index = 1;
            index + 1 < cycle.size() && orientation == 0; ++index){
            orientation = geometry3d_adaptive_detail::sign(
                exact_determinant(
                    directions[cycle[0]],
                    directions[cycle[index]],
                    directions[cycle[index + 1]]
                )
            );
        }
        if(orientation == 0){
            throw std::domain_error("degenerate polar normal fan cell");
        }
        if(orientation < 0){
            std::reverse(cycle.begin() + 1, cycle.end());
        }
        for(std::size_t index = 1; index + 1 < cycle.size(); ++index){
            const std::array<std::size_t, 3> triangle{
                cycle[0], cycle[index], cycle[index + 1]
            };
            if(geometry3d_adaptive_detail::sign(exact_determinant(
                directions[triangle[0]],
                directions[triangle[1]],
                directions[triangle[2]]
            )) <= 0){
                throw std::domain_error(
                    "polar normal fan is not strictly triangulated"
                );
            }
            polar_triangles.push_back(triangle);
            polar_labels.push_back(vertex);
        }
    }
    support_hierarchy_ = RadialHierarchy(
        std::move(directions), polar_triangles, polar_labels
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_BUILD_SUPPORT_HIERARCHY_HPP_INCLUDED
