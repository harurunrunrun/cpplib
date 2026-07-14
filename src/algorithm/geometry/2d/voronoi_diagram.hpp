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

#include "circumcenter.hpp"
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

inline bool point_equal(const Point& first, const Point& second){
    const long double scale = std::max({
        1.0L,
        std::abs(first.x),
        std::abs(first.y),
        std::abs(second.x),
        std::abs(second.y),
    });
    const long double tolerance = GEOMETRY_EPS
        + 64.0L * std::numeric_limits<long double>::epsilon() * scale;
    return std::abs(first.x - second.x) <= tolerance
        && std::abs(first.y - second.y) <= tolerance;
}

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
        const Point center = circumcenter(
            points[triangle[0]], points[triangle[1]], points[triangle[2]]
        );
        if(!std::isfinite(center.x) || !std::isfinite(center.y)){
            throw std::overflow_error("Voronoi vertex is not finite");
        }
        triangle_centers.push_back(center);
    }

    std::vector<std::size_t> center_order(triangle_centers.size());
    for(std::size_t index = 0; index < center_order.size(); ++index){
        center_order[index] = index;
    }
    std::sort(center_order.begin(), center_order.end(), [&](std::size_t first, std::size_t second){
        if(triangle_centers[first].x != triangle_centers[second].x){
            return triangle_centers[first].x < triangle_centers[second].x;
        }
        if(triangle_centers[first].y != triangle_centers[second].y){
            return triangle_centers[first].y < triangle_centers[second].y;
        }
        return first < second;
    });
    std::vector<std::size_t> triangle_vertex(triangle_centers.size());
    for(std::size_t triangle_index: center_order){
        if(result.vertices.empty()
            || !voronoi_diagram_detail::point_equal(
                result.vertices.back(), triangle_centers[triangle_index]
            )){
            result.vertices.push_back(triangle_centers[triangle_index]);
        }
        triangle_vertex[triangle_index] = result.vertices.size() - 1;
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
