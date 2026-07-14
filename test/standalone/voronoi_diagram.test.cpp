// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/voronoi_diagram.hpp"

namespace{

long double squared_distance(const Point& first, const Point& second){
    const Point difference = first - second;
    return dot(difference, difference);
}

bool close(long double first, long double second){
    const long double scale = std::max({1.0L, std::abs(first), std::abs(second)});
    return std::abs(first - second) <= 1.0e-8L * scale;
}

void verify(const std::vector<Point>& points){
    const DelaunayTriangulationResult triangulation = delaunay_triangulation(points);
    const VoronoiDiagramResult diagram = voronoi_diagram(points);
    assert(diagram.representative == triangulation.representative);
    assert(diagram.sites == triangulation.sites);
    assert(diagram.cell_edges.size() == points.size());

    std::set<std::pair<std::size_t, std::size_t>> delaunay_edges(
        triangulation.edges.begin(), triangulation.edges.end()
    );
    std::map<
        std::pair<std::size_t, std::size_t>,
        std::vector<std::size_t>
    > adjacent_triangles;
    for(std::size_t triangle_index = 0;
        triangle_index < triangulation.triangles.size();
        ++triangle_index){
        const auto& triangle = triangulation.triangles[triangle_index];
        for(int index = 0; index < 3; ++index){
            std::size_t first = triangle[static_cast<std::size_t>(index)];
            std::size_t second = triangle[static_cast<std::size_t>((index + 1) % 3)];
            if(first > second) std::swap(first, second);
            adjacent_triangles[{first, second}].push_back(triangle_index);
        }
    }
    const auto assert_pair_is_nearest = [&](
        const Point& query,
        std::size_t first_site,
        std::size_t second_site
    ){
        const long double expected = squared_distance(query, points[first_site]);
        assert(close(expected, squared_distance(query, points[second_site])));
        for(std::size_t site: diagram.sites){
            const long double candidate = squared_distance(query, points[site]);
            assert(expected <= candidate
                + 1.0e-8L * std::max({1.0L, expected, candidate}));
        }
    };
    std::set<std::pair<std::size_t, std::size_t>> voronoi_site_pairs;
    std::map<std::pair<std::size_t, std::size_t>, std::size_t>
        voronoi_edge_by_sites;
    for(std::size_t edge_index = 0; edge_index < diagram.edges.size(); ++edge_index){
        const VoronoiEdge& edge = diagram.edges[edge_index];
        assert(edge.first_site < edge.second_site);
        assert(delaunay_edges.contains({edge.first_site, edge.second_site}));
        assert(voronoi_site_pairs.insert({edge.first_site, edge.second_site}).second);
        voronoi_edge_by_sites[{edge.first_site, edge.second_site}] = edge_index;
        const long double first_distance =
            squared_distance(edge.origin, points[edge.first_site]);
        const long double second_distance =
            squared_distance(edge.origin, points[edge.second_site]);
        assert(close(first_distance, second_distance));
        if(edge.kind == VoronoiEdgeKind::SEGMENT){
            assert(edge.first_vertex < diagram.vertices.size());
            assert(edge.second_vertex < diagram.vertices.size());
            assert(edge.first_vertex != edge.second_vertex);
            assert(edge.origin == diagram.vertices[edge.first_vertex]);
            assert(edge.endpoint_or_direction == diagram.vertices[edge.second_vertex]);
            assert(close(
                squared_distance(edge.endpoint_or_direction, points[edge.first_site]),
                squared_distance(edge.endpoint_or_direction, points[edge.second_site])
            ));
            assert_pair_is_nearest(
                (edge.origin + edge.endpoint_or_direction) / 2.0L,
                edge.first_site,
                edge.second_site
            );
        }else{
            assert(close(dot(
                edge.endpoint_or_direction,
                points[edge.second_site] - points[edge.first_site]
            ), 0.0L));
            assert(close(dot(
                edge.endpoint_or_direction,
                edge.endpoint_or_direction
            ), 1.0L));
            if(edge.kind == VoronoiEdgeKind::RAY){
                assert(edge.first_vertex < diagram.vertices.size());
                assert(edge.second_vertex == VORONOI_NO_VERTEX);
                assert(edge.origin == diagram.vertices[edge.first_vertex]);
                const auto& adjacent = adjacent_triangles.at({
                    edge.first_site, edge.second_site
                });
                assert(adjacent.size() == 1);
                const auto& triangle = triangulation.triangles[adjacent[0]];
                std::size_t opposite = triangle[0];
                for(std::size_t vertex: triangle){
                    if(vertex != edge.first_site && vertex != edge.second_site){
                        opposite = vertex;
                    }
                }
                const Point midpoint = (
                    points[edge.first_site] + points[edge.second_site]
                ) / 2.0L;
                assert(dot(
                    edge.endpoint_or_direction,
                    points[opposite] - midpoint
                ) < 0.0L);
                const long double step = std::max(
                    1.0L,
                    std::sqrt(first_distance)
                );
                assert_pair_is_nearest(
                    edge.origin + edge.endpoint_or_direction * step,
                    edge.first_site,
                    edge.second_site
                );
                const Point inward =
                    edge.origin - edge.endpoint_or_direction * step;
                assert(squared_distance(inward, points[opposite])
                    < squared_distance(inward, points[edge.first_site]));
            }else{
                assert(edge.first_vertex == VORONOI_NO_VERTEX);
                assert(edge.second_vertex == VORONOI_NO_VERTEX);
                const long double line_step = 4.0L * std::max({
                    1.0L,
                    std::abs(points[edge.first_site].x),
                    std::abs(points[edge.first_site].y),
                    std::abs(points[edge.second_site].x),
                    std::abs(points[edge.second_site].y),
                });
                assert_pair_is_nearest(
                    edge.origin + edge.endpoint_or_direction * line_step,
                    edge.first_site,
                    edge.second_site
                );
                assert_pair_is_nearest(
                    edge.origin - edge.endpoint_or_direction * line_step,
                    edge.first_site,
                    edge.second_site
                );
            }
        }
        assert(std::find(
            diagram.cell_edges[edge.first_site].begin(),
            diagram.cell_edges[edge.first_site].end(),
            edge_index
        ) != diagram.cell_edges[edge.first_site].end());
        assert(std::find(
            diagram.cell_edges[edge.second_site].begin(),
            diagram.cell_edges[edge.second_site].end(),
            edge_index
        ) != diagram.cell_edges[edge.second_site].end());
    }

    for(std::size_t site = 0; site < points.size(); ++site){
        if(diagram.representative[site] != site){
            assert(diagram.cell_edges[site].empty());
            continue;
        }
        for(std::size_t edge_index: diagram.cell_edges[site]){
            assert(edge_index < diagram.edges.size());
            const VoronoiEdge& edge = diagram.edges[edge_index];
            const std::size_t representative = diagram.representative[site];
            assert(edge.first_site == representative
                || edge.second_site == representative);
        }
    }

    if(diagram.sites.size() <= 120){
        for(const Point& vertex: diagram.vertices){
            if(diagram.sites.empty()) continue;
            long double minimum = squared_distance(vertex, points[diagram.sites[0]]);
            int equal_count = 0;
            for(std::size_t site: diagram.sites){
                const long double current = squared_distance(vertex, points[site]);
                if(current < minimum && !close(current, minimum)) minimum = current;
            }
            for(std::size_t site: diagram.sites){
                const long double current = squared_distance(vertex, points[site]);
                assert(current + 1.0e-8L * std::max(1.0L, minimum) >= minimum);
                if(close(current, minimum)) ++equal_count;
            }
            assert(equal_count >= 3);
        }
    }

    for(const auto& edge: triangulation.edges){
        const std::size_t adjacent = adjacent_triangles[edge].size();
        const auto iterator = voronoi_edge_by_sites.find(edge);
        if(adjacent == 0){
            assert(iterator != voronoi_edge_by_sites.end());
            assert(diagram.edges[iterator->second].kind == VoronoiEdgeKind::LINE);
        }else if(adjacent == 1){
            assert(iterator != voronoi_edge_by_sites.end());
            assert(diagram.edges[iterator->second].kind == VoronoiEdgeKind::RAY);
        }else if(iterator != voronoi_edge_by_sites.end()){
            assert(diagram.edges[iterator->second].kind == VoronoiEdgeKind::SEGMENT);
        }
    }
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool invalid_thrown = false;
    try{
        static_cast<void>(voronoi_diagram({
            {std::numeric_limits<long double>::quiet_NaN(), 0.0L}
        }));
    }catch(const std::invalid_argument&){
        invalid_thrown = true;
    }
    assert(invalid_thrown);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<Point> points(size);
        for(Point& point: points) std::cin >> point.x >> point.y;
        verify(points);
        std::cout << "OK\n";
    }
}
