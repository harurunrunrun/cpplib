#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "delaunay_triangulation.hpp"
#include "dot.hpp"
#include "rotate90.hpp"
#include "unit.hpp"

enum class VoronoiEdgeKind{
    SEGMENT,
    RAY,
    LINE,
};

inline constexpr std::size_t VORONOI_NO_VERTEX =
    std::numeric_limits<std::size_t>::max();

struct VoronoiEdge{
    VoronoiEdgeKind kind = VoronoiEdgeKind::SEGMENT;
    std::size_t first_site = 0;
    std::size_t second_site = 0;
    Point origin;
    Point endpoint_or_direction;
    std::size_t first_vertex = VORONOI_NO_VERTEX;
    std::size_t second_vertex = VORONOI_NO_VERTEX;
};

struct VoronoiDiagramResult{
    std::vector<Point> vertices;
    std::vector<VoronoiEdge> edges;
    std::vector<std::vector<std::size_t>> cell_edges;
    std::vector<std::size_t> representative;
    std::vector<std::size_t> sites;
};

namespace voronoi_diagram_detail{

class DisjointSet{
public:
    explicit DisjointSet(std::size_t size)
        : parent_(size), size_(size, 1){

        for(std::size_t index = 0; index < size; ++index){
            parent_[index] = index;
        }
    }

    std::size_t find(std::size_t vertex){
        std::size_t root = vertex;
        while(parent_[root] != root) root = parent_[root];
        while(parent_[vertex] != vertex){
            const std::size_t next = parent_[vertex];
            parent_[vertex] = root;
            vertex = next;
        }
        return root;
    }

    void unite(std::size_t first, std::size_t second){
        first = find(first);
        second = find(second);
        if(first == second) return;
        if(size_[first] < size_[second]
            || (size_[first] == size_[second] && first > second)){
            std::swap(first, second);
        }
        parent_[second] = first;
        size_[first] += size_[second];
    }

private:
    std::vector<std::size_t> parent_;
    std::vector<std::size_t> size_;
};

inline Point canonical_direction(Point direction){
    direction = unit(direction);
    if(direction.x < 0.0L
        || (geometry_sign(direction.x) == 0 && direction.y < 0.0L)){
        direction = -direction;
    }
    return direction;
}

inline std::pair<std::size_t, std::size_t> ordered_edge(
    std::size_t first,
    std::size_t second
){
    if(first > second) std::swap(first, second);
    return {first, second};
}

inline std::size_t third_vertex(
    const std::array<std::size_t, 3>& triangle,
    std::size_t first,
    std::size_t second
){
    for(std::size_t vertex: triangle){
        if(vertex != first && vertex != second) return vertex;
    }
    return VORONOI_NO_VERTEX;
}

inline Point delaunay_triangle_circumcenter(
    const Point& first,
    const Point& second,
    const Point& third
){
    if(delaunay_triangulation_detail::orientation(first, second, third) == 0){
        throw std::logic_error("Delaunay triangle is collinear");
    }

    const Point first_direction = second - first;
    const Point second_direction = third - first;
    const long double scale = std::max({
        std::abs(first_direction.x),
        std::abs(first_direction.y),
        std::abs(second_direction.x),
        std::abs(second_direction.y),
    });
    const Point normalized_first = first_direction / scale;
    const Point normalized_second = second_direction / scale;
    const long double denominator = 2.0L * cross(
        normalized_first, normalized_second
    );
    if(denominator == 0.0L){
        throw std::overflow_error("Voronoi circumcenter is not representable");
    }
    const long double first_norm = dot(normalized_first, normalized_first);
    const long double second_norm = dot(normalized_second, normalized_second);
    const Point normalized_offset{
        std::fma(first_norm, normalized_second.y,
                 -second_norm * normalized_first.y) / denominator,
        std::fma(second_norm, normalized_first.x,
                 -first_norm * normalized_second.x) / denominator,
    };
    return first + normalized_offset * scale;
}

}  // namespace voronoi_diagram_detail

inline VoronoiDiagramResult voronoi_diagram(const std::vector<Point>& points){
    const DelaunayTriangulationResult triangulation =
        delaunay_triangulation(points);
    VoronoiDiagramResult result;
    result.representative = triangulation.representative;
    result.sites = triangulation.sites;
    result.cell_edges.resize(points.size());

    std::vector<Point> triangle_centers;
    triangle_centers.reserve(triangulation.triangles.size());
    for(const auto& triangle: triangulation.triangles){
        const Point center = voronoi_diagram_detail::delaunay_triangle_circumcenter(
            points[triangle[0]], points[triangle[1]], points[triangle[2]]
        );
        if(!std::isfinite(center.x) || !std::isfinite(center.y)){
            throw std::overflow_error("Voronoi vertex is not finite");
        }
        triangle_centers.push_back(center);
    }

    std::map<
        std::pair<std::size_t, std::size_t>,
        std::vector<std::size_t>
    > adjacent_triangles;
    for(std::size_t triangle_index = 0;
        triangle_index < triangulation.triangles.size();
        ++triangle_index){
        const auto& triangle = triangulation.triangles[triangle_index];
        for(int edge = 0; edge < 3; ++edge){
            adjacent_triangles[voronoi_diagram_detail::ordered_edge(
                triangle[static_cast<std::size_t>(edge)],
                triangle[static_cast<std::size_t>((edge + 1) % 3)]
            )].push_back(triangle_index);
        }
    }

    voronoi_diagram_detail::DisjointSet components(triangle_centers.size());
    for(const auto& [site_edge, adjacent]: adjacent_triangles){
        if(adjacent.size() < 2) continue;
        const std::size_t first_opposite = voronoi_diagram_detail::third_vertex(
            triangulation.triangles[adjacent[0]],
            site_edge.first,
            site_edge.second
        );
        for(std::size_t index = 1; index < adjacent.size(); ++index){
            const std::size_t second_opposite =
                voronoi_diagram_detail::third_vertex(
                    triangulation.triangles[adjacent[index]],
                    site_edge.first,
                    site_edge.second
                );
            if(delaunay_triangulation_detail::circumcircle_sign(
                points[site_edge.first],
                points[site_edge.second],
                points[first_opposite],
                points[second_opposite]
            ) == 0){
                components.unite(adjacent[0], adjacent[index]);
            }
        }
    }

    std::vector<std::size_t> component_center(
        triangle_centers.size(), VORONOI_NO_VERTEX
    );
    for(std::size_t index = 0; index < triangle_centers.size(); ++index){
        const std::size_t component = components.find(index);
        const std::size_t current = component_center[component];
        if(current == VORONOI_NO_VERTEX
            || triangle_centers[index] < triangle_centers[current]){
            component_center[component] = index;
        }
    }
    std::vector<std::size_t> component_order;
    for(std::size_t component = 0;
        component < component_center.size();
        ++component){
        if(component_center[component] != VORONOI_NO_VERTEX){
            component_order.push_back(component);
        }
    }
    std::sort(component_order.begin(), component_order.end(), [&](
        std::size_t first,
        std::size_t second
    ){
        const Point& first_center = triangle_centers[component_center[first]];
        const Point& second_center = triangle_centers[component_center[second]];
        if(first_center < second_center) return true;
        if(second_center < first_center) return false;
        return first < second;
    });
    std::vector<std::size_t> component_vertex(
        triangle_centers.size(), VORONOI_NO_VERTEX
    );
    for(std::size_t component: component_order){
        component_vertex[component] = result.vertices.size();
        result.vertices.push_back(triangle_centers[component_center[component]]);
    }
    std::vector<std::size_t> triangle_vertex(triangle_centers.size());
    for(std::size_t index = 0; index < triangle_centers.size(); ++index){
        triangle_vertex[index] = component_vertex[components.find(index)];
    }

    for(const auto& site_edge: triangulation.edges){
        const std::size_t first_site = site_edge.first;
        const std::size_t second_site = site_edge.second;
        const auto iterator = adjacent_triangles.find(site_edge);
        const std::vector<std::size_t> empty;
        const std::vector<std::size_t>& adjacent =
            iterator == adjacent_triangles.end() ? empty : iterator->second;

        VoronoiEdge edge;
        edge.first_site = first_site;
        edge.second_site = second_site;
        if(adjacent.size() >= 2){
            const std::size_t first_vertex = triangle_vertex[adjacent[0]];
            const std::size_t second_vertex = triangle_vertex[adjacent[1]];
            if(first_vertex == second_vertex) continue;
            edge.kind = VoronoiEdgeKind::SEGMENT;
            edge.first_vertex = first_vertex;
            edge.second_vertex = second_vertex;
            if(result.vertices[second_vertex] < result.vertices[first_vertex]){
                std::swap(edge.first_vertex, edge.second_vertex);
            }
            edge.origin = result.vertices[edge.first_vertex];
            edge.endpoint_or_direction = result.vertices[edge.second_vertex];
        }else if(adjacent.size() == 1){
            const std::size_t triangle_index = adjacent[0];
            const std::size_t vertex = triangle_vertex[triangle_index];
            const std::size_t opposite = voronoi_diagram_detail::third_vertex(
                triangulation.triangles[triangle_index],
                first_site,
                second_site
            );
            Point direction = unit(rotate90(
                points[second_site] - points[first_site]
            ));
            const Point midpoint = (points[first_site] + points[second_site]) / 2.0L;
            if(dot(direction, points[opposite] - midpoint) > 0.0L){
                direction = -direction;
            }
            edge.kind = VoronoiEdgeKind::RAY;
            edge.origin = result.vertices[vertex];
            edge.endpoint_or_direction = direction;
            edge.first_vertex = vertex;
        }else{
            edge.kind = VoronoiEdgeKind::LINE;
            edge.origin = (points[first_site] + points[second_site]) / 2.0L;
            edge.endpoint_or_direction =
                voronoi_diagram_detail::canonical_direction(rotate90(
                    points[second_site] - points[first_site]
                ));
        }

        const std::size_t edge_index = result.edges.size();
        result.edges.push_back(edge);
        result.cell_edges[first_site].push_back(edge_index);
        result.cell_edges[second_site].push_back(edge_index);
    }

    return result;
}
