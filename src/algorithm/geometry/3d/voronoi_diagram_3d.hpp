#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "adaptive_orient3d.hpp"
#include "circumsphere.hpp"
#include "cross.hpp"
#include "delaunay_tetrahedralization_3d.hpp"
#include "halfspace_intersection_3d.hpp"
#include "is_finite.hpp"
#include "voronoi_diagram3.hpp"

namespace voronoi_diagram_3d_detail{

template<std::size_t Size>
inline std::array<std::size_t, Size> sorted_indices(
    std::array<std::size_t, Size> indices
){
    std::sort(indices.begin(), indices.end());
    return indices;
}

inline Point3 tetrahedron_circumcenter(
    const std::array<std::size_t, 4>& tetrahedron,
    const std::vector<Point3>& points
){
    long double scale = 0.0L;
    for(std::size_t index: tetrahedron){
        scale = std::max({
            scale, std::abs(points[index].x), std::abs(points[index].y),
            std::abs(points[index].z)
        });
    }
    if(scale == 0.0L) scale = 1.0L;
    const Sphere3 sphere = circumsphere({
        points[tetrahedron[0]] / scale, points[tetrahedron[1]] / scale,
        points[tetrahedron[2]] / scale, points[tetrahedron[3]] / scale,
    });
    const Point3 center = sphere.center * scale;
    if(!geometry3d_is_finite(center))[[unlikely]]{
        throw std::overflow_error("3D Voronoi circumcenter overflow");
    }
    return center;
}

template<class Range>
inline void sort_unique(Range& values){
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
}
inline Point3 scaled_difference(const Point3& first, const Point3& second){
    long double scale = std::max({
        std::abs(first.x), std::abs(first.y), std::abs(first.z),
        std::abs(second.x), std::abs(second.y), std::abs(second.z)
    });
    if(scale == 0.0L) scale = 1.0L;
    return first / scale - second / scale;
}


inline std::size_t find_or_add_finite_vertex(
    std::vector<Point3>& vertices,
    const Point3& center
){
    const auto existing = std::find_if(
        vertices.begin(), vertices.end(), [&](const Point3& other){
            const Point3 difference = scaled_difference(center, other);
            return std::hypot(
                difference.x, difference.y, difference.z
            ) <= 64.0L * GEOMETRY3D_EPS;
        }
    );
    if(existing != vertices.end()){
        return static_cast<std::size_t>(existing - vertices.begin());
    }
    vertices.push_back(center);
    return vertices.size() - 1;
}

inline VoronoiRay3 outward_voronoi_ray(
    const std::array<std::size_t, 3>& face,
    std::size_t tetrahedron_index,
    const DelaunayTetrahedralization3& delaunay,
    const std::vector<std::size_t>& tetrahedron_vertices,
    const std::vector<Point3>& finite_vertices
){
    const auto& tetrahedron = delaunay.tetrahedra[tetrahedron_index];
    const auto opposite_iterator = std::find_if(
        tetrahedron.begin(), tetrahedron.end(), [&](std::size_t vertex){
            return std::find(face.begin(), face.end(), vertex) == face.end();
        }
    );
    if(opposite_iterator == tetrahedron.end())[[unlikely]]{
        throw std::logic_error("Delaunay boundary face has no opposite vertex");
    }
    Point3 direction = cross(
        scaled_difference(
            delaunay.vertices[face[1]], delaunay.vertices[face[0]]
        ),
        scaled_difference(
            delaunay.vertices[face[2]], delaunay.vertices[face[0]]
        )
    );
    if(adaptive_orient3d(
        delaunay.vertices[face[0]], delaunay.vertices[face[1]],
        delaunay.vertices[face[2]], delaunay.vertices[*opposite_iterator]
    ) > 0) direction = -direction;
    const long double length = std::hypot(direction.x, direction.y, direction.z);
    if(length == 0.0L)[[unlikely]]{
        throw std::logic_error("degenerate Delaunay boundary face");
    }
    direction /= length;
    const Point3 origin = finite_vertices[tetrahedron_vertices[tetrahedron_index]];
    return {origin, direction};
}

inline std::vector<std::size_t> order_ridge_vertices(
    const std::vector<std::size_t>& edge_indices,
    const std::vector<VoronoiEdge3>& edges,
    const std::vector<std::size_t>& incident,
    bool unbounded
){
    if(incident.size() <= 1) return incident;
    std::map<std::size_t, std::vector<std::size_t>> adjacency;
    std::vector<std::size_t> boundary_endpoints;
    for(std::size_t edge_index: edge_indices){
        const VoronoiEdge3& edge = edges[edge_index];
        if(edge.finite_vertices.size() == 2){
            adjacency[edge.finite_vertices[0]].push_back(edge.finite_vertices[1]);
            adjacency[edge.finite_vertices[1]].push_back(edge.finite_vertices[0]);
        }else if(edge.ray && edge.finite_vertices.size() == 1){
            boundary_endpoints.push_back(edge.finite_vertices.front());
        }
    }
    for(auto& [vertex, neighbors]: adjacency){
        static_cast<void>(vertex);
        sort_unique(neighbors);
    }
    sort_unique(boundary_endpoints);
    std::size_t current = unbounded && !boundary_endpoints.empty()
        ? boundary_endpoints.front() : *std::min_element(incident.begin(), incident.end());
    std::size_t previous = static_cast<std::size_t>(-1);
    std::vector<std::size_t> order;
    while(order.size() < incident.size()){
        order.push_back(current);
        const auto iterator = adjacency.find(current);
        if(iterator == adjacency.end()) break;
        std::size_t next = static_cast<std::size_t>(-1);
        for(std::size_t candidate: iterator->second){
            if(candidate != previous){
                next = candidate;
                break;
            }
        }
        if(next == static_cast<std::size_t>(-1) || next == order.front()) break;
        previous = current;
        current = next;
    }
    std::set<std::size_t> used(order.begin(), order.end());
    for(std::size_t vertex: incident){
        if(!used.contains(vertex)) order.push_back(vertex);
    }
    return order;
}

inline Plane3 cell_bisector_halfspace(
    const Point3& site,
    const Point3& neighbor
){
    return {site / 2.0L + neighbor / 2.0L, scaled_difference(neighbor, site)};
}

}  // namespace voronoi_diagram_3d_detail

inline VoronoiDiagram3 voronoi_diagram_3d(std::vector<Point3> points){
    using namespace voronoi_diagram_3d_detail;
    const DelaunayTetrahedralization3 delaunay =
        delaunay_tetrahedralization_3d(std::move(points));
    VoronoiDiagram3 result;
    result.affine_dimension = delaunay.affine_dimension;
    result.sites = delaunay.vertices;
    result.cells.resize(result.sites.size());
    for(std::size_t site = 0; site < result.cells.size(); ++site){
        result.cells[site].site = site;
    }
    // The following incidence is specifically the full-dimensional 3D dual.
    if(delaunay.affine_dimension < 3) return result;

    std::map<std::array<std::size_t, 3>, std::vector<std::size_t>> face_tetrahedra;
    std::map<std::array<std::size_t, 2>, std::vector<std::size_t>> edge_tetrahedra;
    std::vector<std::size_t> tetrahedron_vertices;
    tetrahedron_vertices.reserve(delaunay.tetrahedra.size());
    for(std::size_t tetrahedron_index = 0;
        tetrahedron_index < delaunay.tetrahedra.size(); ++tetrahedron_index){
        const auto& tetrahedron = delaunay.tetrahedra[tetrahedron_index];
        const std::size_t voronoi_vertex = find_or_add_finite_vertex(
            result.finite_vertices,
            tetrahedron_circumcenter(tetrahedron, result.sites)
        );
        tetrahedron_vertices.push_back(voronoi_vertex);
        for(std::size_t vertex: tetrahedron){
            result.cells[vertex].finite_vertices.push_back(voronoi_vertex);
        }
        for(std::size_t omitted = 0; omitted < 4; ++omitted){
            std::array<std::size_t, 3> face{};
            std::size_t destination = 0;
            for(std::size_t source = 0; source < 4; ++source){
                if(source != omitted) face[destination++] = tetrahedron[source];
            }
            face_tetrahedra[sorted_indices(face)].push_back(tetrahedron_index);
        }
        for(std::size_t first = 0; first < 4; ++first){
            for(std::size_t second = first + 1; second < 4; ++second){
                edge_tetrahedra[sorted_indices(std::array<std::size_t, 2>{
                    tetrahedron[first], tetrahedron[second]
                })].push_back(tetrahedron_index);
            }
        }
    }

    std::map<std::array<std::size_t, 3>, std::size_t> face_edge_index;
    for(auto& [face, incident]: face_tetrahedra){
        sort_unique(incident);
        VoronoiEdge3 edge;
        edge.sites = face;
        for(std::size_t tetrahedron_index: incident){
            edge.finite_vertices.push_back(tetrahedron_vertices[tetrahedron_index]);
        }
        sort_unique(edge.finite_vertices);
        edge.unbounded = incident.size() == 1;
        if(edge.unbounded){
            edge.ray = outward_voronoi_ray(
                face, incident.front(), delaunay, tetrahedron_vertices,
                result.finite_vertices
            );
            for(std::size_t vertex: face) result.cells[vertex].unbounded = true;
        }else if(edge.finite_vertices.size() == 2){
            edge.segment = Segment3{
                result.finite_vertices[edge.finite_vertices[0]],
                result.finite_vertices[edge.finite_vertices[1]],
            };
        }
        face_edge_index[face] = result.edges.size();
        result.edges.push_back(std::move(edge));
    }

    for(auto& [delaunay_edge, incident]: edge_tetrahedra){
        sort_unique(incident);
        std::vector<std::size_t> incident_vertices;
        incident_vertices.reserve(incident.size());
        for(std::size_t tetrahedron_index: incident){
            incident_vertices.push_back(tetrahedron_vertices[tetrahedron_index]);
        }
        sort_unique(incident_vertices);
        VoronoiRidge3 ridge;
        ridge.sites = delaunay_edge;
        for(const auto& [face, edge_index]: face_edge_index){
            if(std::find(face.begin(), face.end(), delaunay_edge[0]) != face.end()
                && std::find(face.begin(), face.end(), delaunay_edge[1]) != face.end()){
                ridge.edge_indices.push_back(edge_index);
                if(result.edges[edge_index].ray){
                    ridge.unbounded_rays.push_back(*result.edges[edge_index].ray);
                }
            }
        }
        ridge.unbounded = !ridge.unbounded_rays.empty();
        ridge.finite_vertices = order_ridge_vertices(
            ridge.edge_indices, result.edges, incident_vertices, ridge.unbounded
        );
        if(ridge.unbounded && ridge.unbounded_rays.size() > 1
            && !ridge.finite_vertices.empty()){
            const Point3& start = result.finite_vertices[ridge.finite_vertices.front()];
            std::stable_sort(
                ridge.unbounded_rays.begin(), ridge.unbounded_rays.end(),
                [&](const VoronoiRay3& left, const VoronoiRay3& right){
                    return left.origin == start && !(right.origin == start);
                }
            );
        }
        const std::size_t ridge_index = result.ridges.size();
        result.cells[delaunay_edge[0]].neighbors.push_back(delaunay_edge[1]);
        result.cells[delaunay_edge[1]].neighbors.push_back(delaunay_edge[0]);
        result.cells[delaunay_edge[0]].ridge_indices.push_back(ridge_index);
        result.cells[delaunay_edge[1]].ridge_indices.push_back(ridge_index);
        result.cells[delaunay_edge[0]].boundary_halfspaces.push_back(
            cell_bisector_halfspace(
                result.sites[delaunay_edge[0]], result.sites[delaunay_edge[1]]
            )
        );
        result.cells[delaunay_edge[1]].boundary_halfspaces.push_back(
            cell_bisector_halfspace(
                result.sites[delaunay_edge[1]], result.sites[delaunay_edge[0]]
            )
        );
        result.ridges.push_back(std::move(ridge));
    }
    for(VoronoiCell3& cell: result.cells){
        sort_unique(cell.finite_vertices);
        if(cell.neighbors.size() != cell.ridge_indices.size())[[unlikely]]{
            throw std::logic_error("Voronoi cell incidence size mismatch");
        }
        const std::set<std::size_t> unique_neighbors(
            cell.neighbors.begin(), cell.neighbors.end()
        );
        if(unique_neighbors.size() != cell.neighbors.size())[[unlikely]]{
            throw std::logic_error("duplicate Voronoi cell neighbor");
        }
        if(!cell.unbounded){
            if(!cell.boundary_halfspaces.empty()){
                ConvexPolyhedron3 bounded = halfspace_intersection_3d(
                    cell.boundary_halfspaces
                );
                if(!bounded.vertices.empty()){
                    if(bounded.affine_dimension == 3){
                        cell.bounded_polyhedron = std::move(bounded);
                    }
                }
            }
        }
    }
    return result;
}
