// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/algorithm/geometry/2d/type/definition/apollonius_diagram.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        ApolloniusDiagram diagram({
            {{0, 0}, 0},
            {{10, 0}, 3},
        });
        assert(diagram.locate({1, 0}).site == 0);
        assert(diagram.curves().size() == 1);
        assert(diagram.edges().size() == 1);
        const Point witness = diagram.curves().front().point(0.0L);
        assert(diagram.on_edge(0, witness));
        ApolloniusDiagram triple({
            {{0, 0}, 0}, {{4, 0}, 0}, {{2, 4}, 0},
        });
        assert(!triple.vertices().empty());
        assert(!triple.edges().empty());
        for(const Point& vertex: triple.vertices()){
            const long double first = std::sqrt(dot(vertex, vertex));
            const long double second = std::sqrt(dot(vertex - Point{4, 0}, vertex - Point{4, 0}));
        ApolloniusDiagram cocircular({
            {{-1, -1}, 0}, {{1, -1}, 0},
            {{1, 1}, 0}, {{-1, 1}, 0},
        });
        assert(cocircular.vertices().size() == 1);
        assert(cocircular.edges().size() == 4);
        for(const auto& cell: cocircular.cell_edges()){
            assert(cell.size() == 2);
        }
            assert(std::abs(first - second) < 1.0e-8L);
        }
        const long double endpoint_weight = std::sqrt(2.0L) - 1.0L;
        ApolloniusDiagram collinear({
            {{-1, 0}, endpoint_weight},
            {{0, 0}, 0},
            {{1, 0}, endpoint_weight},
        });
        bool upper = false;
        bool lower = false;
        for(const Point& vertex: collinear.vertices()){
            upper = upper || plus_geometry_detail::close(vertex, {0, 1});
            lower = lower || plus_geometry_detail::close(vertex, {0, -1});
        }
        assert(upper && lower);
        assert(diagram.locate({8, 0}).site == 1);
        std::cout << "OK\n";
    }
}
