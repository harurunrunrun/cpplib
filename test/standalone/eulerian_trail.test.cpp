// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <optional>
#include <random>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/eulerian_trail.hpp"

bool valid_trail(
    bool directed,
    const std::vector<std::pair<int, int>>& input_edges,
    int start,
    const EulerianTrailResult& trail
){
    const int m = static_cast<int>(input_edges.size());
    if(static_cast<int>(trail.vertices.size()) != m + 1 && m != 0) return false;
    if(static_cast<int>(trail.edges.size()) != m) return false;
    if(m == 0){
        if(trail.vertices.size() > 1) return false;
        return start == -1 || (!trail.vertices.empty() && trail.vertices.front() == start);
    }
    if(static_cast<int>(trail.vertices.size()) != m + 1) return false;
    if(start != -1 && trail.vertices.front() != start) return false;
    std::vector<char> used(static_cast<std::size_t>(m), 0);
    for(int i = 0; i < m; ++i){
        int id = trail.edges[static_cast<std::size_t>(i)];
        if(id < 0 || m <= id || used[static_cast<std::size_t>(id)]) return false;
        used[static_cast<std::size_t>(id)] = 1;
        auto [u, v] = input_edges[static_cast<std::size_t>(id)];
        int from = trail.vertices[static_cast<std::size_t>(i)];
        int to = trail.vertices[static_cast<std::size_t>(i + 1)];
        if(directed){
            if(from != u || to != v) return false;
        }else if(!((from == u && to == v) || (from == v && to == u))){
            return false;
        }
    }
    return true;
}

bool brute_exists(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    bool directed,
    int forced_start
){
    const int m = static_cast<int>(edges.size());
    if(m == 0) return forced_start == -1 || (0 <= forced_start && forced_start < n);
    std::vector<int> starts;
    if(forced_start == -1){
        for(int v = 0; v < n; ++v) starts.push_back(v);
    }else{
        starts.push_back(forced_start);
    }
    for(int start: starts){
        auto dfs = [&](auto&& self, int v, unsigned mask) -> bool {
            if(mask == (1U << m) - 1) return true;
            for(int id = 0; id < m; ++id){
                if((mask >> id) & 1U) continue;
                auto [u, w] = edges[static_cast<std::size_t>(id)];
                if(directed){
                    if(u == v && self(self, w, mask | (1U << id))) return true;
                }else{
                    if(u == v && self(self, w, mask | (1U << id))) return true;
                    if(w == v && self(self, u, mask | (1U << id))) return true;
                }
            }
            return false;
        };
        if(dfs(dfs, start, 0)) return true;
    }
    return false;
}

void self_test(){
    {
        auto trail = directed_eulerian_trail(0, {});
        assert(trail && trail->vertices.empty() && trail->edges.empty());
    }
    {
        std::vector<std::pair<int, int>> edges = {{0, 0}, {0, 1}, {1, 0}, {0, 0}};
        auto trail = directed_eulerian_trail(2, edges);
        assert(trail && valid_trail(true, edges, -1, *trail));
        assert(directed_eulerian_trail(2, edges, 1));
        std::vector<std::pair<int, int>> open = {{0, 1}, {1, 0}, {0, 1}};
        assert(!directed_eulerian_trail(2, open, 1));
    }
    {
        std::vector<std::pair<int, int>> edges = {{0, 1}, {0, 1}, {0, 0}, {1, 1}};
        auto trail = undirected_eulerian_trail(2, edges);
        assert(trail && valid_trail(false, edges, -1, *trail));
    }
    {
        std::vector<std::pair<int, int>> edges = {{0, 1}, {1, 0}, {2, 3}, {3, 2}};
        assert(!directed_eulerian_trail(4, edges));
        assert(!undirected_eulerian_trail(4, edges));
    }

    std::mt19937 rng(20260901);
    for(int n = 1; n <= 5; ++n){
        for(int step = 0; step < 500; ++step){
            int m = static_cast<int>(rng() % 9);
            std::vector<std::pair<int, int>> edges;
            for(int i = 0; i < m; ++i){
                edges.push_back({static_cast<int>(rng() % n), static_cast<int>(rng() % n)});
            }
            int start = rng() % 3 == 0 ? static_cast<int>(rng() % n) : -1;
            for(bool directed: {false, true}){
                auto actual = directed
                    ? directed_eulerian_trail(n, edges, start)
                    : undirected_eulerian_trail(n, edges, start);
                bool expected = brute_exists(n, edges, directed, start);
                assert(actual.has_value() == expected);
                if(actual) assert(valid_trail(directed, edges, start, *actual));
            }
        }
    }
}

int main(){
    int directed, n, m, start;
    if(!(std::cin >> directed >> n >> m >> start)){
        self_test();
        return 0;
    }
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [u, v]: edges) std::cin >> u >> v;
    std::optional<EulerianTrailResult> trail = directed
        ? directed_eulerian_trail(n, edges, start)
        : undirected_eulerian_trail(n, edges, start);
    if(!trail){
        std::cout << 0 << '\n';
    }else{
        std::cout << (valid_trail(directed != 0, edges, start, *trail) ? 1 : -1) << '\n';
    }
}
