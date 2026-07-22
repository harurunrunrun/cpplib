#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM_3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "adaptive_orient3d.hpp"
#include "cross.hpp"
#include "delaunay_tetrahedralization_3d.hpp"
#include "is_finite.hpp"
#include "on_plane.hpp"
#include "voronoi_diagram3.hpp"

namespace voronoi_diagram_3d_detail{

using ExactDyadic3 = std::array<
    geometry3d_adaptive_detail::ExactDyadic, 3
>;

template<std::size_t Size>
struct IndexArrayHash{
    std::size_t operator()(
        const std::array<std::size_t, Size>& indices
    ) const noexcept{
        std::size_t result = 0x9e3779b97f4a7c15ULL;
        for(std::size_t index: indices){
            result ^= std::hash<std::size_t>{}(index)
                + 0x9e3779b97f4a7c15ULL
                + (result << 6) + (result >> 2);
        }
        return result;
    }
};

struct PointHash{
    std::size_t operator()(const Point3& point) const noexcept{
        std::size_t result = 0x517cc1b727220a95ULL;
        for(long double coordinate: {point.x, point.y, point.z}){
            result ^= std::hash<long double>{}(coordinate)
                + 0x9e3779b97f4a7c15ULL
                + (result << 6) + (result >> 2);
        }
        return result;
    }
};

struct PointExactEqual{
    bool operator()(
        const Point3& first,
        const Point3& second
    ) const noexcept{
        return first.x == second.x && first.y == second.y
            && first.z == second.z;
    }
};

inline geometry3d_adaptive_detail::ExactDyadic exact_dot(
    const ExactDyadic3& first,
    const ExactDyadic3& second
){
    using namespace geometry3d_adaptive_detail;
    ExactDyadic result{};
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        result = add(result, multiply(
            first[coordinate], second[coordinate]
        ));
    }
    return result;
}

inline ExactDyadic3 exact_cross(
    const ExactDyadic3& first,
    const ExactDyadic3& second
){
    using namespace geometry3d_adaptive_detail;
    return {
        subtract(
            multiply(first[1], second[2]),
            multiply(first[2], second[1])
        ),
        subtract(
            multiply(first[2], second[0]),
            multiply(first[0], second[2])
        ),
        subtract(
            multiply(first[0], second[1]),
            multiply(first[1], second[0])
        ),
    };
}

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
    using namespace geometry3d_adaptive_detail;
    const Point3& anchor = points[tetrahedron[0]];
    const std::array<ExactDyadic3, 3> directions{
        geometry3d_plane_numeric_detail::exact_difference(
            points[tetrahedron[1]], anchor
        ),
        geometry3d_plane_numeric_detail::exact_difference(
            points[tetrahedron[2]], anchor
        ),
        geometry3d_plane_numeric_detail::exact_difference(
            points[tetrahedron[3]], anchor
        ),
    };
    const std::array<ExactDyadic3, 3> cofactors{
        exact_cross(directions[1], directions[2]),
        exact_cross(directions[2], directions[0]),
        exact_cross(directions[0], directions[1]),
    };
    const ExactDyadic denominator = multiply(
        exact_dyadic(2.0L), exact_dot(directions[0], cofactors[0])
    );
    if(sign(denominator) == 0)[[unlikely]]{
        throw std::logic_error("degenerate Delaunay tetrahedron");
    }

    const std::array<ExactDyadic, 3> squared_lengths{
        exact_dot(directions[0], directions[0]),
        exact_dot(directions[1], directions[1]),
        exact_dot(directions[2], directions[2]),
    };
    const std::array<long double, 3> anchor_coordinates{
        anchor.x, anchor.y, anchor.z,
    };
    std::array<long double, 3> center{};
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        ExactDyadic offset_numerator{};
        for(std::size_t direction_index = 0;
            direction_index < 3; ++direction_index){
            offset_numerator = add(offset_numerator, multiply(
                squared_lengths[direction_index],
                cofactors[direction_index][coordinate]
            ));
        }
        const ExactDyadic center_numerator = add(
            multiply(
                exact_dyadic(anchor_coordinates[coordinate]), denominator
            ),
            offset_numerator
        );
        center[coordinate] =
            geometry3d_plane_numeric_detail::exact_ratio(
                center_numerator,
                denominator,
                "3D Voronoi circumcenter is not representable"
            );
    }
    return {center[0], center[1], center[2]};
}

inline void stable_unique_indices(std::vector<std::size_t>& values){
    std::unordered_set<std::size_t> seen;
    seen.reserve(values.size());
    values.erase(std::remove_if(
        values.begin(), values.end(), [&](std::size_t value){
            return !seen.insert(value).second;
        }
    ), values.end());
}
inline Point3 scaled_difference(const Point3& first, const Point3& second){
    return geometry3d_normalized_difference(first, second).value;
}


template<class IndexMap>
inline std::size_t find_or_add_finite_vertex(
    IndexMap& indices,
    std::vector<Point3>& vertices,
    const Point3& center
){
    const std::size_t next_index = vertices.size();
    const auto [iterator, inserted] =
        indices.try_emplace(center, next_index);
    if(inserted){
        vertices.push_back(center);
    }
    return iterator->second;
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
    std::unordered_map<std::size_t, std::vector<std::size_t>> adjacency;
    adjacency.reserve(incident.size());
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
        stable_unique_indices(neighbors);
    }
    stable_unique_indices(boundary_endpoints);
    std::size_t current = unbounded && !boundary_endpoints.empty()
        ? boundary_endpoints.front() : incident.front();
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
    std::unordered_set<std::size_t> used(order.begin(), order.end());
    for(std::size_t vertex: incident){
        if(!used.contains(vertex)) order.push_back(vertex);
    }
    return order;
}

inline Plane3 cell_bisector_halfspace(
    const Point3& site,
    const Point3& neighbor
){
    return {
        {
            std::midpoint(site.x, neighbor.x),
            std::midpoint(site.y, neighbor.y),
            std::midpoint(site.z, neighbor.z),
        },
        scaled_difference(neighbor, site),
    };
}

inline std::optional<ConvexPolyhedron3>
bounded_cell_polyhedron_from_incidence(
    const VoronoiCell3& cell,
    const VoronoiDiagram3& diagram
){
    if(cell.unbounded || cell.finite_vertices.size() < 4){
        return std::nullopt;
    }
    ConvexPolyhedron3 polyhedron;
    std::unordered_map<std::size_t, std::size_t> local_indices;
    local_indices.reserve(cell.finite_vertices.size());
    polyhedron.vertices.reserve(cell.finite_vertices.size());
    for(std::size_t vertex: cell.finite_vertices){
        local_indices.emplace(vertex, polyhedron.vertices.size());
        polyhedron.vertices.push_back(diagram.finite_vertices[vertex]);
    }
    const auto local_index = [&](std::size_t vertex){
        const auto iterator = local_indices.find(vertex);
        if(iterator == local_indices.end()){
            throw std::logic_error(
                "Voronoi ridge vertex is absent from its incident cell"
            );
        }
        return iterator->second;
    };
    for(std::size_t ridge_index: cell.ridge_indices){
        const VoronoiRidge3& ridge = diagram.ridges[ridge_index];
        // A co-spherical Delaunay triangulation may contain auxiliary edges
        // whose dual ridge has zero area. They are incidence records, not
        // faces of the geometric cell.
        if(ridge.unbounded || ridge.finite_vertices.size() < 3) continue;
        const std::size_t anchor = local_index(ridge.finite_vertices.front());
        for(std::size_t index = 1;
            index + 1 < ridge.finite_vertices.size(); ++index){
            std::array<std::size_t, 3> face{
                anchor,
                local_index(ridge.finite_vertices[index]),
                local_index(ridge.finite_vertices[index + 1]),
            };
            const int site_side = adaptive_orient3d(
                polyhedron.vertices[face[0]],
                polyhedron.vertices[face[1]],
                polyhedron.vertices[face[2]],
                diagram.sites[cell.site]
            );
            if(site_side == 0) continue;
            if(site_side > 0) std::swap(face[1], face[2]);
            polyhedron.faces.push_back(face);
        }
    }
    if(polyhedron.faces.empty()) return std::nullopt;
    polyhedron.affine_dimension = 3;
    return polyhedron;
}

}  // namespace voronoi_diagram_3d_detail

namespace voronoi_diagram_3d_detail{

inline VoronoiDiagram3 from_delaunay(
    DelaunayTetrahedralization3 delaunay
){
    VoronoiDiagram3 result;
    result.affine_dimension = delaunay.affine_dimension;
    result.sites = delaunay.vertices;
    result.cells.resize(result.sites.size());
    for(std::size_t site = 0; site < result.cells.size(); ++site){
        result.cells[site].site = site;
    }
    // The following incidence is specifically the full-dimensional 3D dual.
    if(delaunay.affine_dimension < 3) return result;

    std::unordered_map<
        std::array<std::size_t, 3>, std::vector<std::size_t>,
        IndexArrayHash<3>> face_tetrahedra;
    std::unordered_map<
        std::array<std::size_t, 2>, std::vector<std::size_t>,
        IndexArrayHash<2>> edge_tetrahedra;
    std::vector<std::size_t> tetrahedron_vertices;
    std::unordered_map<
        Point3, std::size_t, PointHash, PointExactEqual
    > finite_vertex_indices;
    face_tetrahedra.reserve(4 * delaunay.tetrahedra.size());
    edge_tetrahedra.reserve(6 * delaunay.tetrahedra.size());
    finite_vertex_indices.reserve(delaunay.tetrahedra.size());
    tetrahedron_vertices.reserve(delaunay.tetrahedra.size());
    for(std::size_t tetrahedron_index = 0;
        tetrahedron_index < delaunay.tetrahedra.size(); ++tetrahedron_index){
        const auto& tetrahedron = delaunay.tetrahedra[tetrahedron_index];
        const std::size_t voronoi_vertex = find_or_add_finite_vertex(
            finite_vertex_indices,
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

    std::unordered_map<
        std::array<std::size_t, 2>, std::vector<std::size_t>,
        IndexArrayHash<2>> edge_face_indices;
    edge_face_indices.reserve(edge_tetrahedra.size());
    for(auto& [face, incident]: face_tetrahedra){
        VoronoiEdge3 edge;
        edge.sites = face;
        for(std::size_t tetrahedron_index: incident){
            edge.finite_vertices.push_back(tetrahedron_vertices[tetrahedron_index]);
        }
        stable_unique_indices(edge.finite_vertices);
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
        const std::size_t edge_index = result.edges.size();
        for(std::size_t first = 0; first < face.size(); ++first){
            for(std::size_t second = first + 1;
                second < face.size(); ++second){
                edge_face_indices[sorted_indices(
                    std::array<std::size_t, 2>{face[first], face[second]}
                )].push_back(edge_index);
            }
        }
        result.edges.push_back(std::move(edge));
    }

    for(auto& [delaunay_edge, incident]: edge_tetrahedra){
        std::vector<std::size_t> incident_vertices;
        incident_vertices.reserve(incident.size());
        for(std::size_t tetrahedron_index: incident){
            incident_vertices.push_back(tetrahedron_vertices[tetrahedron_index]);
        }
        stable_unique_indices(incident_vertices);
        VoronoiRidge3 ridge;
        ridge.sites = delaunay_edge;
        const auto face_indices = edge_face_indices.find(delaunay_edge);
        if(face_indices != edge_face_indices.end()){
            ridge.edge_indices = face_indices->second;
            for(std::size_t edge_index: ridge.edge_indices){
                if(result.edges[edge_index].ray){
                    ridge.unbounded_rays.push_back(
                        *result.edges[edge_index].ray
                    );
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
        stable_unique_indices(cell.finite_vertices);
        if(cell.neighbors.size() != cell.ridge_indices.size())[[unlikely]]{
            throw std::logic_error("Voronoi cell incidence size mismatch");
        }
        const std::unordered_set<std::size_t> unique_neighbors(
            cell.neighbors.begin(), cell.neighbors.end()
        );
        if(unique_neighbors.size() != cell.neighbors.size())[[unlikely]]{
            throw std::logic_error("duplicate Voronoi cell neighbor");
        }
        cell.bounded_polyhedron =
            bounded_cell_polyhedron_from_incidence(cell, result);
    }
    return result;
}

}  // namespace voronoi_diagram_3d_detail

inline VoronoiDiagram3 voronoi_diagram_3d(std::vector<Point3> points){
    return voronoi_diagram_3d_detail::from_delaunay(
        delaunay_tetrahedralization_3d(std::move(points))
    );
}

inline VoronoiDiagram3 voronoi_diagram_3d_randomized(
    std::vector<Point3> points,
    std::uint64_t seed
){
    return voronoi_diagram_3d_detail::from_delaunay(
        delaunay_tetrahedralization_3d_randomized(
            std::move(points), seed
        )
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM_3D_HPP_INCLUDED
