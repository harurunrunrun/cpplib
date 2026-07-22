// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/convex_polyhedron_contains.hpp"
#include "../../src/algorithm/geometry/3d/dot.hpp"
#include "../../src/algorithm/geometry/3d/three_plane_intersection.hpp"
#include "../../src/algorithm/geometry/3d/voronoi_diagram_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

long double point_distance(const Point3& first, const Point3& second){
    return std::hypot(
        first.x - second.x, first.y - second.y, first.z - second.z
    );
}

bool exact_same_point(const Point3& first, const Point3& second){
    return first.x == second.x && first.y == second.y
        && first.z == second.z;
}

std::size_t legacy_find_or_add_finite_vertex(
    std::vector<Point3>& vertices,
    const Point3& center
){
    const auto existing = std::find_if(
        vertices.begin(), vertices.end(), [&](const Point3& other){
            const Point3 difference =
                geometry3d_normalized_difference(center, other).value;
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

std::vector<Point3> legacy_finite_vertices(
    const std::vector<Point3>& sites
){
    const DelaunayTetrahedralization3 delaunay =
        delaunay_tetrahedralization_3d(sites);
    std::vector<Point3> result;
    for(const auto& tetrahedron: delaunay.tetrahedra){
        legacy_find_or_add_finite_vertex(
            result,
            voronoi_diagram_3d_detail::tetrahedron_circumcenter(
                tetrahedron, delaunay.vertices
            )
        );
    }
    return result;
}

bool exact_same_points(
    const std::vector<Point3>& first,
    const std::vector<Point3>& second
){
    if(first.size() != second.size()) return false;
    for(std::size_t index = 0; index < first.size(); ++index){
        if(!exact_same_point(first[index], second[index])) return false;
    }
    return true;
}

bool matches_legacy_finite_vertices(
    const std::vector<Point3>& sites,
    const VoronoiDiagram3& diagram
){
    return exact_same_points(
        diagram.finite_vertices, legacy_finite_vertices(sites)
    );
}

template<class Random>
bool validate_finite_vertex_index(Random& random){
    std::uniform_int_distribution<long long> coordinate(
        -1000000000LL, 1000000000LL
    );
    std::map<Point3, std::size_t> indices;
    std::vector<Point3> indexed_vertices;
    std::vector<Point3> legacy_vertices;
    std::vector<Point3> centers{{0.0L, -0.0L, 0.0L}};
    for(std::size_t index = 0; index < 2000; ++index){
        if(index % 7 == 0){
            centers.push_back(centers[random() % centers.size()]);
        }else{
            centers.push_back({
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
            });
        }
    }
    for(const Point3& center: centers){
        const std::size_t indexed =
            voronoi_diagram_3d_detail::find_or_add_finite_vertex(
                indices, indexed_vertices, center
            );
        const std::size_t legacy =
            legacy_find_or_add_finite_vertex(legacy_vertices, center);
        if(indexed != legacy) return false;
    }
    if(!exact_same_points(indexed_vertices, legacy_vertices)) return false;

    constexpr std::size_t large_size = 50000;
    std::map<Point3, std::size_t> large_indices;
    std::vector<Point3> large_vertices;
    for(std::size_t index = 0; index < large_size; ++index){
        const Point3 center{
            static_cast<long double>(index),
            static_cast<long double>((index * 1000003ULL) % 1000033ULL),
            -static_cast<long double>((index * 1000037ULL) % 1000211ULL),
        };
        if(voronoi_diagram_3d_detail::find_or_add_finite_vertex(
            large_indices, large_vertices, center
        ) != index) return false;
    }
    for(std::size_t index = large_size; index-- > 0;){
        if(voronoi_diagram_3d_detail::find_or_add_finite_vertex(
            large_indices, large_vertices, large_vertices[index]
        ) != index) return false;
    }
    return large_vertices.size() == large_size;
}

bool inside_halfspaces(
    const std::vector<Plane3>& halfspaces,
    const Point3& point
){
    for(const Plane3& halfspace: halfspaces){
        const auto product =
            geometry3d_plane_numeric_detail::exact_dot_difference(
                halfspace.normal, point, halfspace.point
            );
        if(geometry3d_plane_numeric_detail::exact_dot_sign(
            product, 128.0L
        ) > 0) return false;
    }
    return true;
}

std::vector<Point3> brute_force_cell_vertices(
    const VoronoiCell3& cell
){
    std::vector<Point3> result;
    const auto& halfspaces = cell.boundary_halfspaces;
    for(std::size_t first = 0; first < halfspaces.size(); ++first){
        for(std::size_t second = first + 1;
            second < halfspaces.size(); ++second){
            for(std::size_t third = second + 1;
                third < halfspaces.size(); ++third){
                const ThreePlaneIntersection3 intersection =
                    three_plane_intersection(
                        halfspaces[first],
                        halfspaces[second],
                        halfspaces[third]
                    );
                const Point3* point = std::get_if<Point3>(&intersection);
                if(!point || !inside_halfspaces(halfspaces, *point)) continue;
                if(std::none_of(
                    result.begin(), result.end(), [&](const Point3& existing){
                        return geometry3d_api_close(
                            existing, *point, 2e-7L
                        );
                    }
                )){
                    result.push_back(*point);
                }
            }
        }
    }
    return result;
}

bool same_point_set(
    const std::vector<Point3>& first,
    const std::vector<Point3>& second
){
    if(first.size() != second.size()) return false;
    for(const Point3& point: first){
        if(std::none_of(
            second.begin(), second.end(), [&](const Point3& candidate){
                return geometry3d_api_close(point, candidate, 2e-7L);
            }
        )) return false;
    }
    return true;
}

bool validate_bounded_cell(
    const VoronoiDiagram3& diagram,
    const VoronoiCell3& cell,
    bool run_oracle
){
    if(cell.unbounded) return !cell.bounded_polyhedron;
    if(!cell.bounded_polyhedron) return false;
    const ConvexPolyhedron3& polyhedron = *cell.bounded_polyhedron;
    if(polyhedron.affine_dimension != 3
        || polyhedron.vertices.size() != cell.finite_vertices.size()
        || polyhedron.faces.empty()
        || !convex_polyhedron_contains(
            polyhedron, diagram.sites[cell.site]
        )){
        return false;
    }
    std::map<std::array<std::size_t, 2>, std::size_t> edge_counts;
    for(const auto& face: polyhedron.faces){
        if(face[0] >= polyhedron.vertices.size()
            || face[1] >= polyhedron.vertices.size()
            || face[2] >= polyhedron.vertices.size()) return false;
        if(adaptive_orient3d(
            polyhedron.vertices[face[0]],
            polyhedron.vertices[face[1]],
            polyhedron.vertices[face[2]],
            diagram.sites[cell.site]
        ) >= 0) return false;
        for(std::size_t index = 0; index < 3; ++index){
            std::array<std::size_t, 2> edge{
                face[index], face[(index + 1) % 3]
            };
            std::sort(edge.begin(), edge.end());
            ++edge_counts[edge];
        }
    }
    if(std::any_of(
        edge_counts.begin(), edge_counts.end(), [](const auto& entry){
            return entry.second != 2;
        }
    )) return false;
    return !run_oracle || same_point_set(
        polyhedron.vertices, brute_force_cell_vertices(cell)
    );
}

bool validate_voronoi(const VoronoiDiagram3& diagram){
    if(diagram.cells.size() != diagram.sites.size()) return false;
    if(diagram.affine_dimension < 3){
        return diagram.finite_vertices.empty() && diagram.ridges.empty()
            && diagram.edges.empty();
    }
    for(const VoronoiCell3& cell: diagram.cells){
        if(cell.site >= diagram.sites.size()) return false;
        if(cell.neighbors.size() != cell.ridge_indices.size()) return false;
        if(cell.neighbors.size() != cell.boundary_halfspaces.size()) return false;
        if(cell.unbounded){
            if(cell.bounded_polyhedron.has_value()) return false;
        }
        if(!validate_bounded_cell(
            diagram, cell, cell.neighbors.size() <= 14
        )) return false;
        for(std::size_t index = 0; index < cell.neighbors.size(); ++index){
            if(cell.neighbors[index] >= diagram.sites.size()) return false;
            if(cell.ridge_indices[index] >= diagram.ridges.size()) return false;
            const Plane3& halfspace = cell.boundary_halfspaces[index];
            if(dot(
                halfspace.normal, diagram.sites[cell.site] - halfspace.point
            ) > GEOMETRY3D_EPS) return false;
        }
        for(std::size_t vertex: cell.finite_vertices){
            if(vertex >= diagram.finite_vertices.size()) return false;
        }
    }
    for(const VoronoiEdge3& edge: diagram.edges){
        for(std::size_t site: edge.sites){
            if(site >= diagram.sites.size()) return false;
        }
        if(edge.unbounded){
            if(!edge.ray || edge.segment || edge.finite_vertices.size() != 1){
                return false;
            }
            if(!geometry3d_api_close(
                edge.ray->origin,
                diagram.finite_vertices[edge.finite_vertices.front()], 1e-7L
            )) return false;
            for(std::size_t index = 1; index < edge.sites.size(); ++index){
                if(!geometry3d_api_close(
                    point_distance((edge.ray->origin + edge.ray->direction), diagram.sites[edge.sites[0]]),
                    point_distance((edge.ray->origin + edge.ray->direction), diagram.sites[edge.sites[index]]),
                    2e-7L
                )) return false;
            }
        }else{
            if(edge.ray) return false;
            if(edge.finite_vertices.size() == 2){
                if(!edge.segment) return false;
            }else if(edge.finite_vertices.size() == 1){
                if(edge.segment) return false;
            }else{
                return false;
            }
        }
    }
    for(const VoronoiRidge3& ridge: diagram.ridges){
        if(ridge.sites[0] >= diagram.sites.size()) return false;
        if(ridge.sites[1] >= diagram.sites.size()) return false;
        if(ridge.unbounded != !ridge.unbounded_rays.empty()) return false;
        std::vector<std::size_t> legacy_edge_indices;
        for(std::size_t edge_index = 0;
            edge_index < diagram.edges.size(); ++edge_index){
            const auto& sites = diagram.edges[edge_index].sites;
            if(std::find(
                sites.begin(), sites.end(), ridge.sites[0]
            ) != sites.end() && std::find(
                sites.begin(), sites.end(), ridge.sites[1]
            ) != sites.end()){
                legacy_edge_indices.push_back(edge_index);
            }
        }
        if(ridge.edge_indices != legacy_edge_indices) return false;
        for(std::size_t vertex: ridge.finite_vertices){
            if(vertex >= diagram.finite_vertices.size()) return false;
            const Point3& center = diagram.finite_vertices[vertex];
            if(!geometry3d_api_close(
                point_distance(center, diagram.sites[ridge.sites[0]]),
                point_distance(center, diagram.sites[ridge.sites[1]]), 2e-7L
            )) return false;
        }
        for(std::size_t edge_index: ridge.edge_indices){
            if(edge_index >= diagram.edges.size()) return false;
            const auto& sites = diagram.edges[edge_index].sites;
            if(std::find(sites.begin(), sites.end(), ridge.sites[0]) == sites.end()){
                return false;
            }
            if(std::find(sites.begin(), sites.end(), ridge.sites[1]) == sites.end()){
                return false;
            }
        }
    }
    return true;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](auto& random, std::size_t rounds){
        if(!validate_finite_vertex_index(random)) return false;
        const std::vector<Point3> sites{
            {0, 0, 0}, {2, 0, 0}, {0, 2, 0}, {0, 0, 2}, {0.4L, 0.4L, 0.4L}
        };
        const VoronoiDiagram3 diagram = voronoi_diagram_3d(sites);
        if(diagram.affine_dimension != 3 || !validate_voronoi(diagram)) return false;
        if(!matches_legacy_finite_vertices(sites, diagram)) return false;
        if(std::none_of(diagram.cells.begin(), diagram.cells.end(), [](const auto& cell){
            return cell.bounded_polyhedron.has_value();
        })) return false;
        const VoronoiDiagram3 randomized =
            voronoi_diagram_3d_randomized(sites, random());
        if(randomized.affine_dimension != 3
            || !validate_voronoi(randomized)) return false;

        const auto cospherical_cube = voronoi_diagram_3d({
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
            {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1}
        });
        if(!validate_voronoi(cospherical_cube)) return false;
        if(cospherical_cube.finite_vertices.size() != 1) return false;
        if(!matches_legacy_finite_vertices(
            cospherical_cube.sites, cospherical_cube
        )) return false;

        if(rounds == 32){
            // The central cell has one facet per shell site. Re-running a
            // generic high-degree halfspace intersection for this cell would
            // dominate the Delaunay construction.
            constexpr std::size_t shell_size = 240;
            const long double golden_angle =
                std::acos(-1.0L) * (3.0L - std::sqrt(5.0L));
            std::vector<Point3> high_degree_sites{{0, 0, 0}};
            high_degree_sites.reserve(shell_size + 1);
            for(std::size_t index = 0; index < shell_size; ++index){
                const long double z = 1.0L - 2.0L
                    * (static_cast<long double>(index) + 0.5L)
                    / static_cast<long double>(shell_size);
                const long double radial =
                    std::sqrt(std::max(0.0L, 1.0L - z * z));
                const long double angle =
                    golden_angle * static_cast<long double>(index);
                high_degree_sites.push_back({
                    4.0L * radial * std::cos(angle),
                    4.0L * radial * std::sin(angle),
                    4.0L * z,
                });
            }
            const VoronoiDiagram3 high_degree =
                voronoi_diagram_3d(high_degree_sites);
            const auto center = std::find(
                high_degree.sites.begin(),
                high_degree.sites.end(),
                Point3{0, 0, 0}
            );
            if(center == high_degree.sites.end()) return false;
            const VoronoiCell3& center_cell = high_degree.cells[
                static_cast<std::size_t>(center - high_degree.sites.begin())
            ];
            if(center_cell.neighbors.size() != shell_size
                || !validate_bounded_cell(
                    high_degree, center_cell, false
                )) return false;
        }

        const long double translation = 1e3000L;
        const long double ulp = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        ) - translation;
        const long double upper = translation + 64 * ulp;
        std::vector<Point3> thin_box_sites;
        for(int x = 0; x < 2; ++x){
            for(int y = 0; y < 2; ++y){
                for(int z = 0; z < 2; ++z){
                    thin_box_sites.push_back({
                        static_cast<long double>(x),
                        static_cast<long double>(y),
                        z ? upper : translation,
                    });
                }
            }
        }
        const auto translated_thin_box =
            voronoi_diagram_3d(thin_box_sites);
        if(translated_thin_box.affine_dimension != 3
            || !validate_voronoi(translated_thin_box)
            || translated_thin_box.finite_vertices.size() != 1){
            return false;
        }
        const Point3& translated_center =
            translated_thin_box.finite_vertices.front();
        if(translated_center.x != 0.5L
            || translated_center.y != 0.5L
            || translated_center.z != translation + 32 * ulp){
            return false;
        }

        const auto flat = voronoi_diagram_3d({
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}
        });
        if(flat.affine_dimension != 2 || !validate_voronoi(flat)) return false;

        std::uniform_real_distribution<long double> coordinate(-5.0L, 5.0L);
        const std::size_t iterations = std::min<std::size_t>(rounds, 20);
        for(std::size_t iteration = 0; iteration < iterations; ++iteration){
            std::vector<Point3> random_sites;
            const std::size_t size = 5 + random() % 7;
            for(std::size_t index = 0; index < size; ++index){
                random_sites.push_back({
                    coordinate(random), coordinate(random),
                    coordinate(random) + static_cast<long double>(index) * 1e-6L
                });
            }
            const VoronoiDiagram3 random_diagram =
                voronoi_diagram_3d(random_sites);
            if(random_diagram.affine_dimension != 3) return false;
            if(!validate_voronoi(random_diagram)) return false;
            if(!matches_legacy_finite_vertices(
                random_sites, random_diagram
            )) return false;
        }
        return true;
    });
}
