// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/almost_shortest_path.hpp"

using Edge = AlmostShortestPathEdge<long long>;

void self_test(){
    {
        std::vector<Edge> edges = {
            {0, 1, 1}, {0, 2, 1}, {0, 3, 2}, {0, 4, 3},
            {1, 5, 2}, {2, 6, 4}, {3, 6, 2}, {4, 6, 4},
            {5, 6, 1},
        };
        auto result = almost_shortest_path<long long>(7, edges, 0, 6);
        assert(result.reachable && result.distance == 5);
        assert((result.removed == std::vector<char>{
            1, 0, 1, 0, 1, 0, 1, 0, 1
        }));
    }
    {
        std::vector<Edge> edges = {
            {0, 1, 1}, {0, 1, 1}, {0, 1, 3},
        };
        auto result = almost_shortest_path<long long>(2, edges, 0, 1);
        assert(result.reachable && result.distance == 3);
        assert((result.removed == std::vector<char>{1, 1, 0}));
    }
    {
        std::vector<Edge> edges = {
            {0, 1, 0}, {0, 2, 0}, {1, 3, 1}, {2, 3, 1},
            {0, 3, 2},
        };
        auto result = almost_shortest_path<long long>(4, edges, 0, 3);
        assert(result.reachable && result.distance == 2);
        assert((result.removed == std::vector<char>{1, 1, 1, 1, 0}));
    }
    {
        auto result = almost_shortest_path<long long>(3, {{0, 1, 4}}, 0, 2);
        assert(!result.reachable);
        assert((result.removed == std::vector<char>{0}));
    }
    {
        auto result = almost_shortest_path<long long>(2, {{0, 1, 4}}, 0, 1);
        assert(!result.reachable);
        assert((result.removed == std::vector<char>{1}));
    }
    {
        std::vector<Edge> edges = {{0, 1, 0}, {1, 0, 0}, {0, 2, 1}};
        auto result = almost_shortest_path<long long>(3, edges, 0, 0);
        assert(result.reachable && result.distance == 0);
        assert((result.removed == std::vector<char>{0, 0, 0}));
    }
    {
        bool thrown = false;
        try{
            (void)almost_shortest_path<long long>(2, {{0, 1, -1}}, 0, 1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
}

int main(){
    self_test();

    int n, m, source, target;
    if(!(std::cin >> n >> m >> source >> target)) return 0;
    std::vector<Edge> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int edge_index = 0; edge_index < m; edge_index++){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        edges.push_back({from, to, cost});
    }
    auto result = almost_shortest_path<long long>(
        n, edges, source, target
    );
    std::cout << (result.reachable ? result.distance : -1) << '\n';
}
