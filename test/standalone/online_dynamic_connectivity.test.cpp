// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "../../src/structure/graph/online_dynamic_connectivity_internal.hpp"
#include "../../src/structure/graph/online_dynamic_connectivity.hpp"

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

int naive_groups(int n, const std::map<std::pair<int, int>, int>& edge_count){
    std::vector<std::vector<int>> graph(n);
    for(auto [edge, count]: edge_count){
        if(count == 0) continue;
        auto [u, v] = edge;
        if(u == v) continue;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }
    std::vector<int> seen(n, 0);
    int result = 0;
    for(int s = 0; s < n; s++){
        if(seen[s]) continue;
        result++;
        std::queue<int> que;
        seen[s] = 1;
        que.push(s);
        while(!que.empty()){
            int v = que.front();
            que.pop();
            for(int to: graph[v]){
                if(!seen[to]){
                    seen[to] = 1;
                    que.push(to);
                }
            }
        }
    }
    return result;
}

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        OnlineDynamicConnectivity<256> graph(input_n);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "ADD"){
                int u, v;
                std::cin >> u >> v;
                std::cout << graph.add_edge(u, v) << '\n';
            }else if(type == "ERASE"){
                int u, v;
                std::cin >> u >> v;
                std::cout << graph.erase_edge(u, v) << '\n';
            }else if(type == "SAME"){
                int u, v;
                std::cin >> u >> v;
                std::cout << graph.same(u, v) << '\n';
            }else if(type == "SIZE"){
                int v;
                std::cin >> v;
                std::cout << graph.component_size(v) << '\n';
            }else if(type == "MULT"){
                int u, v;
                std::cin >> u >> v;
                std::cout << graph.edge_multiplicity(u, v) << '\n';
            }else if(type == "GROUPS"){
                std::cout << graph.groups() << '\n';
            }
        }
        return 0;
    }

    {
        OnlineDynamicConnectivity<1> empty(0);
        assert(empty.size() == 0 && empty.groups() == 0);
        assert(empty.active_levels() == 1);
    }
    {
        OnlineDynamicConnectivity<1> graph(1);
        assert(graph.same(0, 0));
        assert(graph.component_size(0) == 1);
        assert(graph.add_edge(0, 0));
        assert(!graph.add_edge(0, 0));
        assert(graph.edge_multiplicity(0, 0) == 2);
        assert(graph.erase_edge(0, 0));
        assert(graph.erase_edge(0, 0));
        assert(graph.groups() == 1);
    }
    {
        OnlineDynamicConnectivity<5> graph(4);
        assert(!graph.same(0, 1));
        assert(graph.link(0, 1));
        assert(graph.link(1, 2));
        assert(graph.same(0, 2));
        assert(graph.link(0, 2));
        assert(graph.cut(1, 2));
        assert(graph.same(0, 2));
        assert(graph.cut(0, 2));
        assert(!graph.same(0, 2));
        assert(graph.groups() == 3);
        assert(!graph.cut(0, 2));
    }
    {
        OnlineDynamicConnectivity<3> graph(2);
        assert(graph.add_edge(0, 1));
        assert(!graph.add_edge(0, 1));
        assert(graph.edge_multiplicity(1, 0) == 2);
        assert(graph.erase_edge(0, 1));
        assert(graph.same(0, 1));
        assert(graph.erase_edge(0, 1));
        assert(!graph.same(0, 1));
    }

    {
        constexpr int replacement_n = 2048;
        OnlineDynamicConnectivity<replacement_n> graph(replacement_n);
        for(int vertex = 0; vertex + 1 < replacement_n; ++vertex){
            assert(graph.add_edge(vertex, vertex + 1));
        }
        for(int vertex = 0; vertex + 2 < replacement_n; ++vertex){
            assert(graph.add_edge(vertex, vertex + 2));
        }
        assert(graph.add_edge(0, replacement_n - 1));
        for(int vertex = 0; vertex + 1 < replacement_n; ++vertex){
            assert(graph.erase_edge(vertex, vertex + 1));
            if(vertex % 31 == 0){
                assert(graph.same(0, replacement_n - 1));
                assert(graph.groups() == 1);
                assert(graph.component_size(vertex) == replacement_n);
            }
        }
        assert(graph.groups() == 1);
        assert(graph.active_levels() >= 2);
        assert(graph.erase_edge(0, replacement_n - 1));
        assert(graph.groups() == 2);
        assert(graph.component_size(0) == replacement_n / 2);
        assert(graph.component_size(1) == replacement_n / 2);
    }

    constexpr int n = 35;
    OnlineDynamicConnectivity<50> graph(n);
    std::mt19937 rng(1234567);
    std::map<std::pair<int, int>, int> edge_count;
    std::vector<std::pair<int, int>> active_edges;

    auto normalize = [](int u, int v){
        if(v < u) std::swap(u, v);
        return std::pair<int, int>{u, v};
    };

    for(int step = 0; step < 3000; step++){
        int type = static_cast<int>(rng() % 10);
        if(type < 5 || active_edges.empty()){
            int u = static_cast<int>(rng() % n);
            int v = static_cast<int>(rng() % n);
            auto edge = normalize(u, v);
            graph.add_edge(edge.first, edge.second);
            edge_count[edge]++;
            active_edges.push_back(edge);
        }else{
            int id = static_cast<int>(rng() % active_edges.size());
            auto edge = active_edges[static_cast<std::size_t>(id)];
            assert(graph.erase_edge(edge.first, edge.second));
            edge_count[edge]--;
            active_edges[static_cast<std::size_t>(id)] = active_edges.back();
            active_edges.pop_back();
        }

        for(int k = 0; k < 8; k++){
            int u = static_cast<int>(rng() % n);
            int v = static_cast<int>(rng() % n);
            assert(graph.same(u, v) == naive_same(u, v, n, edge_count));
        }
        int sample = static_cast<int>(rng() % n);
        int expected_size = 0;
        for(int v = 0; v < n; v++){
            expected_size += naive_same(sample, v, n, edge_count);
        }
        assert(graph.component_size(sample) == expected_size);

        int a = static_cast<int>(rng() % n);
        int b = static_cast<int>(rng() % n);
        auto sampled_edge = normalize(a, b);
        assert(graph.edge_multiplicity(a, b) == edge_count[sampled_edge]);
        assert(graph.groups() == naive_groups(n, edge_count));
    }
}
