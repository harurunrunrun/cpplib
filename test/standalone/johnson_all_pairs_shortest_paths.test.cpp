// competitive-verifier: STANDALONE

#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/johnson_all_pairs_shortest_paths.hpp"

namespace{

using Cost = long long;
using Edge = JohnsonEdge<Cost>;
using Result = JohnsonAllPairsShortestPathsResult<Cost>;
using Wide = __int128_t;

constexpr Cost inf = (Cost(1) << 60);

void require(bool condition){
    if(!condition) throw std::runtime_error("Johnson standalone test failed");
}

void validate_path(
    const Result& result,
    const std::vector<Edge>& edges,
    int source,
    int target
){
    const auto path = result.path(source, target);
    if(!result.reachable[static_cast<std::size_t>(source)]
                        [static_cast<std::size_t>(target)]){
        require(path.empty());
        return;
    }
    require(!path.empty());
    require(path.front() == source && path.back() == target);
    require(path.size() <= result.dist.size());

    Wide total = Wide(0);
    for(std::size_t index = 1; index < path.size(); ++index){
        bool found = false;
        Cost minimum = 0;
        for(const auto& edge: edges){
            if(edge.from != path[index - 1] || edge.to != path[index]){
                continue;
            }
            if(!found || edge.cost < minimum){
                found = true;
                minimum = edge.cost;
            }
        }
        require(found);
        total += static_cast<Wide>(minimum);
    }
    require(total == static_cast<Wide>(
        result.dist[static_cast<std::size_t>(source)]
                   [static_cast<std::size_t>(target)]
    ));
}

void self_test(){
    {
        const std::vector<Edge> edges{
            {0, 1, 2}, {0, 2, 5}, {1, 2, -4}, {2, 3, 3},
            {1, 3, 8}, {0, 1, 7}, {2, 2, 0},
        };
        const auto result = johnson_all_pairs_shortest_paths(5, edges, inf);
        require(result.dist[0][3] == 1);
        require(result.dist[1][3] == -1);
        require(!result.reachable[4][0]);
        require(result.dist[4][0] == inf);
        require(result.path(4, 0).empty());
        validate_path(result, edges, 0, 3);
        validate_path(result, edges, 4, 4);
    }
    {
        const std::vector<Edge> edges{{0, 1, inf}};
        const auto result = johnson_all_pairs_shortest_paths(2, edges, inf);
        require(result.reachable[0][1]);
        require(result.dist[0][1] == inf);
        require(result.path(0, 1) == std::vector<int>({0, 1}));
    }
    {
        // The diamond fixes an equal-distance parent by heap vertex order.  The
        // zero-cost 1 <-> 2 cycle must never produce a parent cycle.
        const std::vector<Edge> first{
            {0, 2, 1}, {2, 3, 1}, {2, 1, 0},
            {0, 1, 1}, {1, 3, 1}, {1, 2, 0},
        };
        std::vector<Edge> reversed = first;
        std::reverse(reversed.begin(), reversed.end());
        const auto lhs = johnson_all_pairs_shortest_paths(4, first, inf);
        const auto rhs = johnson_all_pairs_shortest_paths(4, reversed, inf);
        require(lhs.dist == rhs.dist);
        require(lhs.reachable == rhs.reachable);
        require(lhs.parent == rhs.parent);
        require(lhs.path(0, 3) == std::vector<int>({0, 1, 3}));
        for(int source = 0; source < 4; ++source){
            for(int target = 0; target < 4; ++target){
                validate_path(lhs, first, source, target);
            }
        }
    }
    {
        const auto empty = johnson_all_pairs_shortest_paths<Cost>(0, {}, inf);
        require(empty.dist.empty());
        require(empty.parent.empty());
        require(empty.reachable.empty());
        const auto singleton = johnson_all_pairs_shortest_paths<Cost>(
            1, {{0, 0, 0}, {0, 0, 3}}, inf
        );
        require(singleton.dist[0][0] == 0);
        require(singleton.path(0, 0) == std::vector<int>({0}));
    }

    bool thrown = false;
    try{
        (void)johnson_all_pairs_shortest_paths<Cost>(
            4, {{2, 3, -2}, {3, 2, 1}}, inf
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    require(thrown);

    thrown = false;
    try{
        (void)johnson_all_pairs_shortest_paths<Cost>(
            3,
            {{0, 1, std::numeric_limits<Cost>::max()}, {1, 2, 1}},
            inf
        );
    }catch(const std::overflow_error&){
        thrown = true;
    }
    require(thrown);

    thrown = false;
    try{
        (void)johnson_all_pairs_shortest_paths<Cost>(
            3,
            {{0, 1, std::numeric_limits<Cost>::lowest()}, {1, 2, -1}},
            inf
        );
    }catch(const std::overflow_error&){
        thrown = true;
    }
    require(thrown);

    thrown = false;
    try{
        (void)johnson_all_pairs_shortest_paths<Cost>(2, {{0, 2, 1}}, inf);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    require(thrown);

    thrown = false;
    try{
        (void)johnson_all_pairs_shortest_paths<Cost>(-1, {}, inf);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    require(thrown);

    thrown = false;
    try{
        const auto result = johnson_all_pairs_shortest_paths<Cost>(1, {}, inf);
        (void)result.path(0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    require(thrown);
}

} // namespace

int main(){
    self_test();

    std::string mode;
    if(!(std::cin >> mode)) return 0;
    int vertex_count, edge_count, query_count;
    std::cin >> vertex_count >> edge_count >> query_count;
    std::vector<Edge> edges(static_cast<std::size_t>(edge_count));
    for(auto& edge: edges){
        std::cin >> edge.from >> edge.to >> edge.cost;
    }
    std::vector<std::pair<int, int>> queries(
        static_cast<std::size_t>(query_count)
    );
    for(auto& [source, target]: queries) std::cin >> source >> target;

    if(mode == "NEGATIVE"){
        try{
            (void)johnson_all_pairs_shortest_paths(vertex_count, edges, inf);
        }catch(const std::runtime_error&){
            std::cout << "NEGATIVE_CYCLE\n";
            return 0;
        }
        return 2;
    }
    if(mode == "OVERFLOW"){
        try{
            (void)johnson_all_pairs_shortest_paths(vertex_count, edges, inf);
        }catch(const std::overflow_error&){
            std::cout << "OVERFLOW\n";
            return 0;
        }
        return 3;
    }
    require(mode == "DISTANCE");
    const auto result = johnson_all_pairs_shortest_paths(
        vertex_count, edges, inf
    );
    std::cout << "OK\n";
    for(const auto [source, target]: queries){
        validate_path(result, edges, source, target);
        if(result.reachable[static_cast<std::size_t>(source)]
                           [static_cast<std::size_t>(target)]){
            std::cout << result.dist[static_cast<std::size_t>(source)]
                                    [static_cast<std::size_t>(target)] << '\n';
        }else{
            std::cout << "INF\n";
        }
    }
}
