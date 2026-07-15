// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/dot.hpp"
#include "../../src/algorithm/geometry/3d/voronoi_diagram_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

long double point_distance(const Point3& first, const Point3& second){
    return std::hypot(
        first.x - second.x, first.y - second.y, first.z - second.z
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
        const std::vector<Point3> sites{
            {0, 0, 0}, {2, 0, 0}, {0, 2, 0}, {0, 0, 2}, {0.4L, 0.4L, 0.4L}
        };
        const VoronoiDiagram3 diagram = voronoi_diagram_3d(sites);
        if(diagram.affine_dimension != 3 || !validate_voronoi(diagram)) return false;
        if(std::none_of(diagram.cells.begin(), diagram.cells.end(), [](const auto& cell){
            return cell.bounded_polyhedron.has_value();
        })) return false;

        const auto cospherical_cube = voronoi_diagram_3d({
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
            {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1}
        });
        if(!validate_voronoi(cospherical_cube)) return false;
        if(cospherical_cube.finite_vertices.size() != 1) return false;

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
        }
        return true;
    });
}
