// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "../../src/algorithm/graph/specialized/minimum_steiner_tree.hpp"

constexpr long long minimum_steiner_infinity =
    std::numeric_limits<long long>::max() / 4;

struct MinimumSteinerDsu{
    std::vector<int> parent;
    explicit MinimumSteinerDsu(int size): parent(static_cast<std::size_t>(size)){
        std::iota(parent.begin(), parent.end(), 0);
    }
    int leader(int vertex){
        if(parent[static_cast<std::size_t>(vertex)] == vertex) return vertex;
        return parent[static_cast<std::size_t>(vertex)] = leader(
            parent[static_cast<std::size_t>(vertex)]
        );
    }
    void merge(int left, int right){
        left = leader(left);
        right = leader(right);
        if(left != right) parent[static_cast<std::size_t>(right)] = left;
    }
};

long long brute_minimum_steiner_tree(
    int vertex_count,
    const std::vector<std::tuple<int, int, long long>>& edges,
    const std::vector<int>& terminals
){
    if(terminals.empty()) return 0;
    long long answer = minimum_steiner_infinity;
    const int edge_count = static_cast<int>(edges.size());
    for(int subset = 0; subset < (1 << edge_count); ++subset){
        MinimumSteinerDsu dsu(vertex_count);
        long long cost = 0;
        for(int edge = 0; edge < edge_count; ++edge){
            if(((subset >> edge) & 1) == 0) continue;
            const auto [left, right, weight] = edges[static_cast<std::size_t>(edge)];
            dsu.merge(left, right);
            cost += weight;
        }
        const int root = dsu.leader(terminals[0]);
        bool connected = true;
        for(int terminal: terminals){
            if(dsu.leader(terminal) != root) connected = false;
        }
        if(connected && cost < answer) answer = cost;
    }
    return answer;
}

void validate_minimum_steiner_result(
    int vertex_count,
    const std::vector<std::tuple<int, int, long long>>& edges,
    const std::vector<int>& terminals,
    const MinimumSteinerTreeResult<long long>& result
){
    if(!result.exists) return;
    MinimumSteinerDsu dsu(vertex_count);
    std::unordered_set<int> used;
    long long cost = 0;
    for(int id: result.edge_ids){
        assert(0 <= id && id < static_cast<int>(edges.size()));
        assert(used.insert(id).second);
        const auto [left, right, weight] = edges[static_cast<std::size_t>(id)];
        dsu.merge(left, right);
        cost += weight;
    }
    assert(cost == result.cost);
    if(!terminals.empty()){
        const int root = dsu.leader(terminals[0]);
        for(int terminal: terminals) assert(dsu.leader(terminal) == root);
    }
}

MinimumSteinerTreeResult<long long> solve_minimum_steiner(
    int vertex_count,
    const std::vector<std::tuple<int, int, long long>>& edges,
    const std::vector<int>& terminals
){
    std::vector<std::vector<MinimumSteinerTreeEdge<long long>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(int id = 0; id < static_cast<int>(edges.size()); ++id){
        const auto [left, right, cost] = edges[static_cast<std::size_t>(id)];
        graph[static_cast<std::size_t>(left)].push_back({right, cost, id});
        graph[static_cast<std::size_t>(right)].push_back({left, cost, id});
    }
    return minimum_steiner_tree(graph, terminals, minimum_steiner_infinity);
}

void self_test(){
    {
        const std::vector<std::tuple<int, int, long long>> edges{
            {0, 1, 2}, {1, 2, 2}, {1, 3, 1}, {3, 4, 1}, {2, 4, 10}
        };
        const std::vector<int> terminals{0, 2, 4};
        const auto result = solve_minimum_steiner(5, edges, terminals);
        assert(result.exists && result.cost == 6);
        validate_minimum_steiner_result(5, edges, terminals, result);
    }
    {
        const std::vector<std::tuple<int, int, long long>> edges{{0, 1, 3}};
        const std::vector<int> terminals{0, 2};
        const auto result = solve_minimum_steiner(3, edges, terminals);
        assert(!result.exists);
    }

    std::mt19937 random(2026071311);
    for(int vertex_count = 1; vertex_count <= 8; ++vertex_count){
        for(int iteration = 0; iteration < 100; ++iteration){
            std::vector<std::tuple<int, int, long long>> edges;
            for(int left = 0; left < vertex_count; ++left){
                for(int right = left + 1; right < vertex_count; ++right){
                    if(random() % 4 == 0 && edges.size() < 15){
                        edges.push_back({left, right, 1 + random() % 20});
                    }
                }
            }
            std::vector<int> terminals;
            for(int vertex = 0; vertex < vertex_count; ++vertex){
                if(random() % 3 == 0 && terminals.size() < 5){
                    terminals.push_back(vertex);
                }
            }
            if(terminals.empty()){
                terminals.push_back(static_cast<int>(
                    random() % static_cast<unsigned>(vertex_count)
                ));
            }
            const auto result = solve_minimum_steiner(
                vertex_count, edges, terminals
            );
            const long long expected = brute_minimum_steiner_tree(
                vertex_count, edges, terminals
            );
            assert(result.exists == (expected != minimum_steiner_infinity));
            if(result.exists){
                assert(result.cost == expected);
                validate_minimum_steiner_result(
                    vertex_count, edges, terminals, result
                );
            }
        }
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count, terminal_count;
    if(!(std::cin >> vertex_count >> edge_count >> terminal_count)){
        self_test();
        return 0;
    }
    std::vector<int> terminals(static_cast<std::size_t>(terminal_count));
    for(int& terminal: terminals) std::cin >> terminal;
    std::vector<std::tuple<int, int, long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    for(int edge = 0; edge < edge_count; ++edge){
        int left, right;
        long long cost;
        std::cin >> left >> right >> cost;
        edges.push_back({left, right, cost});
    }
    const auto result = solve_minimum_steiner(
        vertex_count, edges, terminals
    );
    if(result.exists){
        validate_minimum_steiner_result(
            vertex_count, edges, terminals, result
        );
        std::cout << result.cost << '\n';
    }else{
        std::cout << -1 << '\n';
    }
}
