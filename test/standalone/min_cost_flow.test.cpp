// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "../../src/algorithm/matching/min_cost_flow.hpp"

std::pair<long long, long long> brute(
    int n,
    int source,
    int sink,
    const std::vector<std::tuple<int, int, long long, long long>>& edges,
    long long flow_limit = (1LL << 60)
){
    std::vector<long long> flow(edges.size(), 0);
    long long best_flow = -1;
    long long best_cost = 0;
    auto dfs = [&](auto&& self, int id) -> void {
        if(id == static_cast<int>(edges.size())){
            std::vector<long long> balance(static_cast<std::size_t>(n), 0);
            long long cost = 0;
            for(std::size_t i = 0; i < edges.size(); i++){
                auto [u, v, cap, edge_cost] = edges[i];
                (void)cap;
                balance[static_cast<std::size_t>(u)] -= flow[i];
                balance[static_cast<std::size_t>(v)] += flow[i];
                cost += flow[i] * edge_cost;
            }
            for(int v = 0; v < n; v++){
                if(v == source || v == sink) continue;
                if(balance[static_cast<std::size_t>(v)] != 0) return;
            }
            long long sent = -balance[static_cast<std::size_t>(source)];
            if(balance[static_cast<std::size_t>(sink)] != sent || sent > flow_limit) return;
            if(best_flow < sent || (best_flow == sent && cost < best_cost)){
                best_flow = sent;
                best_cost = cost;
            }
            return;
        }
        auto [u, v, cap, cost] = edges[static_cast<std::size_t>(id)];
        (void)u;
        (void)v;
        (void)cost;
        for(long long f = 0; f <= cap; f++){
            flow[static_cast<std::size_t>(id)] = f;
            self(self, id + 1);
        }
    };
    dfs(dfs, 0);
    return {best_flow, best_cost};
}

void self_test(){
    {
        bool thrown = false;
        try{
            [[maybe_unused]] MinCostFlow<int> graph(-1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        MinCostFlow<long long> graph(4);
        graph.add_edge(0, 1, 2, 1);
        graph.add_edge(0, 2, 1, 5);
        graph.add_edge(1, 2, 1, -2);
        graph.add_edge(1, 3, 1, 2);
        graph.add_edge(2, 3, 2, 1);
        auto res = graph.min_cost_flow(0, 3);
        assert(res.flow == 3);
        assert(res.cost == 9);
    }
    {
        MinCostFlow<long long> graph(2);
        graph.add_edge(0, 0, 3, 4);
        assert(graph.graph[0].size() == 2);
        assert(graph.graph[0][0].rev == 1);
        assert(graph.graph[0][1].rev == 0);
    }
    {
        constexpr long long large_cost = 3000000000000000000LL;
        MinCostFlow<long long> graph(2);
        graph.add_edge(0, 1, 1, large_cost);
        auto result = graph.min_cost_flow(0, 1, 1);
        assert(result.flow == 1);
        assert(result.cost == large_cost);
    }
    {
        MinCostFlow<long long> graph(1);
        bool threw = false;
        try{
            (void)graph.min_cost_flow(0, 0);
        }catch(const std::runtime_error&){
            threw = true;
        }
        assert(threw);
    }
    {
        MinCostFlow<long long> graph(4);
        graph.add_edge(0, 1, 1, 0);
        graph.add_edge(1, 2, 1, -1);
        graph.add_edge(2, 1, 1, 0);
        graph.add_edge(2, 3, 1, 0);
        bool threw = false;
        try{
            (void)graph.min_cost_flow(0, 3);
        }catch(const std::runtime_error&){
            threw = true;
        }
        assert(threw);
    }
    {
        MinCostFlow<long long> graph(4);
        graph.add_edge(0, 1, 2, -3);
        graph.add_edge(1, 3, 2, 5);
        graph.add_edge(0, 2, 1, 7);
        graph.add_edge(2, 3, 1, 0);
        const auto first = graph.min_cost_flow(0, 3, 1);
        const auto second = graph.min_cost_flow(0, 3, 2);
        const auto exhausted = graph.min_cost_flow(0, 3);
        assert(first.flow == 1 && first.cost == 2);
        assert(second.flow == 2 && second.cost == 9);
        assert(exhausted.flow == 0 && exhausted.cost == 0);
    }
    {
        MinCostFlow<long long> graph(4);
        graph.add_edge(0, 1, 1, 0);
        graph.add_edge(1, 2, 1, -1);
        graph.add_edge(2, 1, 1, 0);
        graph.add_edge(2, 3, 1, 0);
        const auto result = graph.min_cost_flow(0, 3, 0);
        assert(result.flow == 0);
        assert(result.cost == 0);
        assert(graph.edges[0].flow == 0);
    }
    std::mt19937 rng(20260827);
    for(int n = 2; n <= 5; n++){
        for(int step = 0; step < 80; step++){
            int source = 0, sink = n - 1;
            MinCostFlow<long long> graph(n);
            std::vector<std::tuple<int, int, long long, long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u < v && rng() % 4 == 0 && static_cast<int>(edges.size()) < 8){
                        long long cap = static_cast<int>(rng() % 4);
                        long long cost = static_cast<int>(rng() % 11) - 5;
                        graph.add_edge(u, v, cap, cost);
                        edges.push_back({u, v, cap, cost});
                    }
                }
            }
            auto expected = brute(n, source, sink, edges);
            auto actual = graph.min_cost_flow(source, sink);
            assert(actual.flow == expected.first);
            assert(actual.cost == expected.second);
        }
    }

    for(int n = 2; n <= 5; n++){
        for(int step = 0; step < 250; step++){
            int source = 0, sink = n - 1;
            std::vector<long long> potential(static_cast<std::size_t>(n));
            for(long long& value: potential){
                value = static_cast<int>(rng() % 11) - 5;
            }
            MinCostFlow<long long> graph(n);
            std::vector<std::tuple<int, int, long long, long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(rng() % 5 != 0 || static_cast<int>(edges.size()) == 7) continue;
                    long long cap = static_cast<int>(rng() % 3);
                    long long cost = static_cast<int>(rng() % 6)
                                   + potential[static_cast<std::size_t>(v)]
                                   - potential[static_cast<std::size_t>(u)];
                    graph.add_edge(u, v, cap, cost);
                    edges.push_back({u, v, cap, cost});
                }
            }
            long long flow_limit = static_cast<int>(rng() % 5);
            auto expected = brute(n, source, sink, edges, flow_limit);
            auto actual = graph.min_cost_flow(source, sink, flow_limit);
            assert(actual.flow == expected.first);
            assert(actual.cost == expected.second);

            std::vector<long long> balance(static_cast<std::size_t>(n), 0);
            long long restored_cost = 0;
            for(const auto& edge: graph.edges){
                assert(0 <= edge.flow && edge.flow <= edge.cap);
                balance[static_cast<std::size_t>(edge.from)] -= edge.flow;
                balance[static_cast<std::size_t>(edge.to)] += edge.flow;
                restored_cost += edge.flow * edge.cost;
            }
            assert(-balance[static_cast<std::size_t>(source)] == actual.flow);
            assert(balance[static_cast<std::size_t>(sink)] == actual.flow);
            for(int v = 1; v + 1 < n; v++){
                assert(balance[static_cast<std::size_t>(v)] == 0);
            }
            assert(restored_cost == actual.cost);
        }
    }
}

int main(){
    int n, m, s, t;
    if(!(std::cin >> n >> m >> s >> t)){
        self_test();
        return 0;
    }
    MinCostFlow<long long> graph(n);
    for(int i = 0; i < m; i++){
        int u, v;
        long long cap, cost;
        std::cin >> u >> v >> cap >> cost;
        graph.add_edge(u, v, cap, cost);
    }
    auto res = graph.min_cost_flow(s, t);
    std::cout << res.flow << ' ' << res.cost << '\n';
}
