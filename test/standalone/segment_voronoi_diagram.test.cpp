// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include "../../src/algorithm/geometry/2d/type/definition/segment_voronoi_diagram.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    std::mt19937_64 random(0x5e67aULL);
    while(cases-- > 0){
        SegmentVoronoiDiagram diagram({
            {{0, 0}, {4, 0}},
            {{0, 5}, {4, 5}},
        });
        assert(diagram.locate({2, 1}).segment == 0);
        assert(!diagram.vertices().empty());
        assert(!diagram.edges().empty());
        bool found = false;
        for(std::size_t edge = 0; edge < diagram.edges().size(); ++edge){
            found = found || diagram.on_edge(edge, {2, 2.5L});
        }
        assert(found);
        SegmentVoronoiDiagram point_sites({
            {{0, 0}, {0, 0}}, {{2, 0}, {2, 0}},
        });
        assert(point_sites.edges().size() == 1);
        assert(point_sites.on_edge(0, {1, 3}));
        SegmentVoronoiDiagram shared_endpoint({
            {{0, 0}, {2, 0}}, {{2, 0}, {2, 2}},
        });
        assert(!shared_endpoint.edges().empty());
        bool rejected_duplicate = false;
        try{
            static_cast<void>(SegmentVoronoiDiagram({
                {{0, 0}, {2, 0}}, {{2, 0}, {0, 0}},
            }));
        }catch(const std::invalid_argument&){
            rejected_duplicate = true;
        }
        assert(rejected_duplicate);
        bool rejected_overlap = false;
        try{
            static_cast<void>(SegmentVoronoiDiagram({
                {{0, 0}, {3, 0}}, {{1, 0}, {4, 0}},
            }));
        }catch(const std::invalid_argument&){
            rejected_overlap = true;
        }
        assert(rejected_overlap);
        bool rejected_nonfinite = false;
        try{
            static_cast<void>(diagram.on_edge(
                0,
                {std::numeric_limits<long double>::quiet_NaN(), 0}
            ));
        }catch(const std::invalid_argument&){
            rejected_nonfinite = true;
        }
        assert(rejected_nonfinite);
        std::uniform_real_distribution<long double> coordinate(-8.0L, 8.0L);
        for(int repetition = 0; repetition < 20; ++repetition){
            std::vector<Segment> sites(3);
            for(Segment& site: sites){
                do{
                    site = {
                        {coordinate(random), coordinate(random)},
                        {coordinate(random), coordinate(random)},
                    };
                }while(plus_geometry_detail::close(site.a, site.b));
            }
            const SegmentVoronoiDiagram random_diagram(std::move(sites));
            for(const SegmentVoronoiEdge& edge: random_diagram.edges()){
                if(edge.first_vertex != SegmentVoronoiEdge::no_vertex){
                    assert(random_diagram.on_edge(
                        &edge - random_diagram.edges().data(),
                        random_diagram.vertices()[edge.first_vertex]
                    ));
                }
                if(edge.second_vertex != SegmentVoronoiEdge::no_vertex){
                    assert(random_diagram.on_edge(
                        &edge - random_diagram.edges().data(),
                        random_diagram.vertices()[edge.second_vertex]
                    ));
                }
                if(std::isfinite(edge.first_parameter)
                    && std::isfinite(edge.second_parameter)){
                    assert(plus_geometry_detail::sign(
                        edge.second_parameter - edge.first_parameter,
                        std::max({
                            1.0L,
                            std::abs(edge.first_parameter),
                            std::abs(edge.second_parameter),
                        })
                    ) > 0);
                }
            }
        }
        assert(diagram.locate({2, 4}).segment == 1);
        std::cout << "OK\n";
    }
}
