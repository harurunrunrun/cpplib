// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "../../src/structure/graph/offline_dynamic_connectivity.hpp"

bool naive_same(
    int s,
    int t,
    int n,
    const std::map<std::pair<int, int>, int>& edge_count
){
    std::vector<std::vector<int>> graph(n);
    for(auto [edge, count]: edge_count){
        if(count == 0) continue;
        auto [u, v] = edge;
        if(u == v) continue;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }
    std::vector<int> seen(n, 0);
    std::queue<int> que;
    seen[s] = 1;
    que.push(s);
    while(!que.empty()){
        int v = que.front();
        que.pop();
        if(v == t) return true;
        for(int to: graph[v]){
            if(!seen[to]){
                seen[to] = 1;
                que.push(to);
            }
        }
    }
    return false;
}

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        OfflineDynamicConnectivity<128, 2048, 8192> dc(input_n, input_q);
        struct Query{ int type = 0, u = 0, v = 0; };
        std::vector<Query> queries(static_cast<std::size_t>(input_q));
        for(int time = 0; time < input_q; time++){
            std::string type;
            int u, v;
            std::cin >> type;
            if(type == "ADD"){
                std::cin >> u >> v;
                dc.add_edge(time, u, v);
            }else if(type == "ERASE"){
                std::cin >> u >> v;
                dc.erase_edge(time, u, v);
            }else if(type == "SAME"){
                std::cin >> u >> v;
                queries[static_cast<std::size_t>(time)] = {1, u, v};
            }else if(type == "SIZE"){
                std::cin >> u;
                queries[static_cast<std::size_t>(time)] = {2, u, 0};
            }else if(type == "GROUPS"){
                queries[static_cast<std::size_t>(time)] = {3, 0, 0};
            }
        }
        dc.run([&](int time, const auto& dsu){
            const auto& query = queries[static_cast<std::size_t>(time)];
            if(query.type == 1) std::cout << dsu.same(query.u, query.v) << '\n';
            else if(query.type == 2) std::cout << dsu.component_size(query.u) << '\n';
            else if(query.type == 3) std::cout << dsu.groups() << '\n';
        });
        return 0;
    }

    {
        OfflineDynamicConnectivity<2, 4, 20> dc(2, 4);
        assert(dc.size() == 2);
        assert(dc.time_size() == 4);
        dc.add_edge(0, 0, 1);
        dc.add_edge(1, 0, 1);
        dc.erase_edge(2, 0, 1);
        dc.erase_edge(3, 0, 1);
        std::vector<bool> expected = {true, true, true, false};
        dc.run([&](int time, const auto& dsu){
            assert(dsu.same(0, 1) == expected[static_cast<std::size_t>(time)]);
        });
    }

    constexpr int n = 30;
    constexpr int q = 400;
    OfflineDynamicConnectivity<40, 500, 20000> dc(n, q);
    std::mt19937 rng(998244353);

    std::map<std::pair<int, int>, int> edge_count;
    std::vector<std::pair<int, int>> active_edges;
    std::vector<std::vector<std::tuple<int, int, bool>>> queries(q);

    auto normalize = [](int u, int v){
        if(v < u) std::swap(u, v);
        return std::pair<int, int>{u, v};
    };

    for(int time = 0; time < q; time++){
        int type = static_cast<int>(rng() % 10);
        if(type < 5 || active_edges.empty()){
            int u = static_cast<int>(rng() % n);
            int v = static_cast<int>(rng() % n);
            auto edge = normalize(u, v);
            dc.add_edge(time, edge.first, edge.second);
            edge_count[edge]++;
            active_edges.push_back(edge);
        }else{
            int id = static_cast<int>(rng() % active_edges.size());
            auto edge = active_edges[static_cast<std::size_t>(id)];
            dc.erase_edge(time, edge.first, edge.second);
            edge_count[edge]--;
            active_edges[static_cast<std::size_t>(id)] = active_edges.back();
            active_edges.pop_back();
        }

        for(int k = 0; k < 4; k++){
            int u = static_cast<int>(rng() % n);
            int v = static_cast<int>(rng() % n);
            bool expected = naive_same(u, v, n, edge_count);
            queries[static_cast<std::size_t>(time)].push_back({u, v, expected});
        }
    }

    dc.run([&](int time, const auto& dsu){
        for(auto [u, v, expected]: queries[static_cast<std::size_t>(time)]){
            assert(dsu.same(u, v) == expected);
        }
    });

    {
        OfflineDynamicConnectivity<2, 4, 4> empty(0, 0);
        assert(empty.size() == 0);
        assert(empty.time_size() == 0);
        int calls = 0;
        empty.run([&](int, const auto&){ ++calls; });
        assert(calls == 0);
        empty.run([&](int, const auto&){ ++calls; });
        assert(calls == 0);
    }
    {
        bool thrown = false;
        try{
            OfflineDynamicConnectivity<2, 4, 4> invalid(3, 1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        thrown = false;
        try{
            OfflineDynamicConnectivity<2, 4, 4> invalid(1, 5);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);

        OfflineDynamicConnectivity<2, 4, 8> checked(2, 2);
        thrown = false;
        try{ checked.add_edge(2, 0, 1); }
        catch(const std::runtime_error&){ thrown = true; }
        assert(thrown);
        thrown = false;
        try{ checked.add_edge(0, -1, 1); }
        catch(const std::runtime_error&){ thrown = true; }
        assert(thrown);
        thrown = false;
        try{ checked.erase_edge(0, 0, 1); }
        catch(const std::runtime_error&){ thrown = true; }
        assert(thrown);

        checked.add_edge(0, 0, 1);
        checked.run([](int, const auto&){});
        thrown = false;
        try{ checked.add_edge(1, 0, 1); }
        catch(const std::runtime_error&){ thrown = true; }
        assert(thrown);
        thrown = false;
        try{ checked.erase_edge(1, 0, 1); }
        catch(const std::runtime_error&){ thrown = true; }
        assert(thrown);
    }
}
