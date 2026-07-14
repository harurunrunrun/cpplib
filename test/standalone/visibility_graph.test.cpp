// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/visibility_graph.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        int vertex_count;
        std::cin >> vertex_count;
        std::vector<Point> polygon(static_cast<std::size_t>(vertex_count));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        const auto edges = visibility_graph(polygon);
        std::cout << edges.size() << '\n';
        for(const VisibilityGraphEdge& edge: edges){
            assert(edge.first < edge.second);
            const long double expected = std::sqrt(
                norm(polygon[edge.second] - polygon[edge.first])
            );
            assert(std::fabs(edge.distance - expected) <= 1e-9L);
            std::cout << edge.first << ' ' << edge.second << '\n';
        }
    }
}
