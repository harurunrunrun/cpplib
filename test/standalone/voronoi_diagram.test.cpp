// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/type/definition/voronoi_edge_kind.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/voronoi_edge.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/voronoi_diagram_result.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set/voronoi_diagram_points.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/voronoi_diagram.hpp"

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
                // 半直線は opposite と反対側を向く（ほぼ共線な三角形でも判定できるよう、
                // 辺の法線との内積と opposite の向きの符号で比べる）。
                const Point site_direction =
                    points[edge.second_site] - points[edge.first_site];
                const long double normal_component = dot(
                    edge.endpoint_or_direction,
                    Point{-site_direction.y, site_direction.x}
                );
                const int opposite_side = delaunay_triangulation_detail::orientation(
                    points[edge.first_site],
                    points[edge.second_site],
                    points[opposite]
                );
                assert(opposite_side != 0);
                assert((normal_component > 0.0L) != (opposite_side > 0));
                const long double step = 2.0L * std::max(
                    1.0L,
                    std::sqrt(first_distance)
                );
                assert_pair_is_nearest(
                    edge.origin + edge.endpoint_or_direction * step,
                    edge.first_site,
                    edge.second_site
                );
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

    const VoronoiEdge default_edge;
    assert(default_edge.kind == VoronoiEdgeKind::SEGMENT);
    assert(default_edge.first_site == 0 && default_edge.second_site == 0);
    assert(default_edge.origin == Point{});
    assert(default_edge.endpoint_or_direction == Point{});
    assert(default_edge.first_vertex == VORONOI_NO_VERTEX);
    assert(default_edge.second_vertex == VORONOI_NO_VERTEX);
    const VoronoiDiagramResult default_result;
    assert(default_result.vertices.empty() && default_result.edges.empty());
    assert(default_result.cell_edges.empty());
    assert(default_result.representative.empty() && default_result.sites.empty());

    bool invalid_thrown = false;
    try{
        static_cast<void>(voronoi_diagram({
            {std::numeric_limits<long double>::quiet_NaN(), 0.0L}
        }));
    }catch(const std::invalid_argument&){
        invalid_thrown = true;
    }
    assert(invalid_thrown);

    const long double large_scale = 1.0e12L;
    const VoronoiDiagramResult large_cocircular = voronoi_diagram({
        {5.0L * large_scale, 0.0L},
        {3.0L * large_scale, 4.0L * large_scale},
        {0.0L, 5.0L * large_scale},
        {-3.0L * large_scale, 4.0L * large_scale},
        {-5.0L * large_scale, 0.0L},
        {-3.0L * large_scale, -4.0L * large_scale},
        {0.0L, -5.0L * large_scale},
        {3.0L * large_scale, -4.0L * large_scale},
    });
    assert(large_cocircular.vertices.size() == 1);
    assert(large_cocircular.edges.size() == 8);
    assert(std::all_of(
        large_cocircular.edges.begin(),
        large_cocircular.edges.end(),
        [](const VoronoiEdge& edge){
            return edge.kind == VoronoiEdgeKind::RAY;
        }
    ));

    const VoronoiDiagramResult almost_cocircular = voronoi_diagram({
        {0.0L, 0.0L},
        {large_scale, 0.0L},
        {large_scale, large_scale + 1.0e-4L},
        {0.0L, large_scale},
    });
    assert(almost_cocircular.vertices.size() == 2);
    assert(almost_cocircular.edges.size() == 5);
    assert(std::count_if(
        almost_cocircular.edges.begin(),
        almost_cocircular.edges.end(),
        [](const VoronoiEdge& edge){
            return edge.kind == VoronoiEdgeKind::SEGMENT;
        }
    ) == 1);

    // 半径を相対 1e-16 程度だけ揺らした単位円上の点。修正前は分割統治の併合が
    // 終わらず std::bad_alloc になっていた。
    const std::vector<Point> perturbed_circle{
        {0xd.8f0281fc9f346cap-4L, -0x8.7eb26de8b395aa9p-4L},
        {-0xd.d0585eb0e1e89bep-5L, -0xe.6eb248d83ee680fp-4L},
        {0x9.1a70277f1d61c6fp-4L, 0xd.286c50f3f511f5fp-4L},
        {0xf.ff2bed306b6600ap-4L, -0xa.4c01376fd4182e3p-9L},
        {-0xe.361d471d08f161ep-4L, -0xe.b3b089d5f9866f7p-5L},
        {-0xe.d0765a3b25bf797p-5L, -0xe.2ea303bf0f5b79ep-4L},
        {-0xc.317fa594cd8c4a7p-4L, 0xa.5c114bddada2968p-4L},
        {0xd.82d953ad75c627cp-5L, 0xe.80fddcf70206de4p-4L},
        {-0xc.bd58b61a982f082p-4L, -0x9.ae121f060304cf5p-4L},
        {0xe.b7edbfb470550b2p-4L, 0xc.8c6e8012e9bfb42p-5L},
        {-0xe.311b8290c1c53a6p-6L, -0xf.9a06d4a0a83a8aap-4L},
        {-0xa.1cd5895e02f35adp-4L, 0xc.662366bd0f21f78p-4L},
        {0xf.f9cde4a65d9312fp-4L, 0xe.13455006f51467p-8L},
        {-0xb.78f23e0d914c534p-4L, -0xb.27181af3d22ddfcp-4L},
    };
    verify(perturbed_circle);
    assert(delaunay_triangulation(perturbed_circle).triangles.size() == 12);

    // ほぼ共円・ほぼ共線な点集合をランダムに生成して検証する。
    std::mt19937_64 random(20261114);
    const auto uniform = [&](long double low, long double high){
        return low + (high - low)
            * std::uniform_real_distribution<long double>(0.0L, 1.0L)(random);
    };
    const long double pi = std::acos(-1.0L);
    for(int iteration = 0; iteration < 3000; ++iteration){
        const int kind = iteration % 3;
        const int size = 3 + static_cast<int>(random() % 30);
        const long double noise = std::array<long double, 5>{
            0.0L, 1.0e-16L, 4.0e-16L, 1.6e-15L, 1.0e-13L
        }[random() % 5];
        std::vector<Point> points;
        if(kind < 2){
            const Point center{uniform(-10.0L, 10.0L), uniform(-10.0L, 10.0L)};
            const long double radius = std::pow(10.0L, uniform(-3.0L, 3.0L));
            const long double rotation = uniform(0.0L, 2.0L * pi);
            for(int index = 0; index < size; ++index){
                const long double angle = kind == 0
                    ? uniform(0.0L, 2.0L * pi)
                    : rotation + 2.0L * pi * index / size;
                const long double current = radius * (1.0L + noise * uniform(-1.0L, 1.0L));
                points.push_back(center + Point{
                    current * std::cos(angle), current * std::sin(angle)
                });
            }
            if(random() % 3 == 0) points.push_back(center);
        }else{
            const Point origin{uniform(-5.0L, 5.0L), uniform(-5.0L, 5.0L)};
            const long double angle = uniform(0.0L, pi);
            const Point direction{std::cos(angle), std::sin(angle)};
            for(int index = 0; index < size; ++index){
                points.push_back(origin + direction * uniform(-10.0L, 10.0L) + Point{
                    noise * uniform(-1.0L, 1.0L), noise * uniform(-1.0L, 1.0L)
                });
            }
            if(random() % 2 == 0){
                points.push_back({uniform(-10.0L, 10.0L), uniform(-10.0L, 10.0L)});
            }
        }
        if(random() % 4 == 0) points.push_back(points[random() % points.size()]);
        verify(points);
    }

    int test_count = 0;
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
