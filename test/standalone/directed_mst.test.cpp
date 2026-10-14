// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/spanning_tree/directed_mst.hpp"

constexpr long long INF = (1LL << 60);

void check_certificate(int n, int root, const std::vector<DirectedMstEdge<long long>>& edges,
                       const DirectedMstResult<long long>& result){
    assert(result.exists);
    assert(result.parent[root] == root && result.parent_edge[root] == -1);
    long long sum = 0;
    for(int vertex = 0; vertex < n; ++vertex){
        if(vertex == root) continue;
        const int index = result.parent_edge[vertex];
        assert(index >= 0 && index < static_cast<int>(edges.size()));
        assert(edges[index].to == vertex && edges[index].from == result.parent[vertex]);
        sum += edges[index].cost;
        int current = vertex, steps = 0;
        while(current != root && steps <= n){
            current = result.parent[current];
            assert(current >= 0 && current < n);
            ++steps;
        }
        assert(current == root && steps <= n);
    }
    assert(sum == result.cost);
}

std::pair<long long, bool> brute(
    int n,
    int root,
    const std::vector<DirectedMstEdge<long long>>& edges
){
    const int m = static_cast<int>(edges.size());
    long long best = 0;
    bool found = false;
    for(int mask = 0; mask < (1 << m); mask++){
        if(__builtin_popcount(static_cast<unsigned>(mask)) != n - 1) continue;
        std::vector<int> indegree(static_cast<std::size_t>(n), 0);
        std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
        long long cost = 0;
        bool ok = true;
        for(int i = 0; i < m; i++){
            if(mask >> i & 1){
                const auto& e = edges[static_cast<std::size_t>(i)];
                indegree[static_cast<std::size_t>(e.to)]++;
                graph[static_cast<std::size_t>(e.from)].push_back(e.to);
                cost += e.cost;
            }
        }
        if(indegree[static_cast<std::size_t>(root)] != 0) ok = false;
        for(int v = 0; v < n; v++){
            if(v != root && indegree[static_cast<std::size_t>(v)] != 1) ok = false;
        }
        std::vector<char> seen(static_cast<std::size_t>(n), 0);
        std::vector<int> stack{root};
        seen[static_cast<std::size_t>(root)] = 1;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            for(int to: graph[static_cast<std::size_t>(v)]){
                if(!seen[static_cast<std::size_t>(to)]){
                    seen[static_cast<std::size_t>(to)] = 1;
                    stack.push_back(to);
                }
            }
        }
        for(int v = 0; v < n; v++){
            if(!seen[static_cast<std::size_t>(v)]) ok = false;
        }
        if(ok && (!found || cost < best)){
            best = cost;
            found = true;
        }
    }
    return {best, found};
}

void self_test(){
    {
        std::vector<DirectedMstEdge<long long>> edges = {
            {0, 1, 5},
            {0, 2, 3},
            {2, 1, 1},
            {1, 3, 2},
            {2, 3, 10},
        };
        auto res = directed_mst<long long>(4, 0, edges, INF);
        assert(res.exists);
        assert(res.cost == 6);
    }
    {
        std::vector<DirectedMstEdge<long long>> edges = {{1, 0, 1}};
        auto res = directed_mst<long long>(2, 0, edges, INF);
        assert(!res.exists);
    }
    {
        std::vector<DirectedMstEdge<long long>> edges = {
            {0, 0, -100},
            {0, 1, 5},
            {0, 1, 2},
            {1, 2, -4},
            {2, 1, 0},
            {0, 2, 10},
        };
        auto expected = brute(3, 0, edges);
        auto actual = directed_mst<long long>(3, 0, edges, INF);
        assert(actual.exists == expected.second);
        assert(actual.cost == expected.first);
    }
    {
        std::vector<DirectedMstEdge<long long>> edges = {
            {0, 1, INF},
            {0, 2, 3000000000000000000LL},
        };
        auto res = directed_mst<long long>(3, 0, edges, INF);
        assert(res.exists);
        assert(res.cost == INF + 3000000000000000000LL);
    }
    std::mt19937 rng(20260816);
    for(int n = 1; n <= 7; n++){
        for(int step = 0; step < 80; step++){
            std::vector<DirectedMstEdge<long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u != v && rng() % 3 == 0 && static_cast<int>(edges.size()) < 20){
                        edges.push_back({u, v, static_cast<int>(rng() % 30) - 10});
                    }
                }
            }
            int root = static_cast<int>(rng() % n);
            auto expected = brute(n, root, edges);
            auto actual = directed_mst<long long>(n, root, edges, INF);
            assert(actual.exists == expected.second);
            if(actual.exists){
                assert(actual.cost == expected.first);
                check_certificate(n, root, edges, actual);
            }
            const std::vector<int> roots{0, n - 1};
            const auto choice = directed_mst_with_root_choices(n, roots, edges);
            bool found = false;
            long long best = 0;
            for(int candidate: roots){
                const auto trial = brute(n, candidate, edges);
                if(trial.second && (!found || trial.first < best)){
                    found = true;
                    best = trial.first;
                }
            }
            assert(choice.tree.exists == found);
            if(found){
                assert(choice.tree.cost == best);
                check_certificate(n, choice.root, edges, choice.tree);
            }
        }
    }
}

int main(){
    int n, m, root;
    if(!(std::cin >> n >> m >> root)){
        self_test();
        return 0;
    }
    std::vector<DirectedMstEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        edges.push_back({u, v, w});
    }
    auto res = directed_mst<long long>(n, root, edges, INF);
    std::cout << (res.exists ? res.cost : -1) << '\n';
}
