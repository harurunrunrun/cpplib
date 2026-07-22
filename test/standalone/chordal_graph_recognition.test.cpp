// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/constraints/chordal_graph_recognition.hpp"

using EdgeSet = std::set<std::pair<int, int>>;

EdgeSet make_edge_set(const std::vector<std::pair<int, int>>& edges){
    EdgeSet result;
    for(auto [left, right]: edges){
        if(left > right) std::swap(left, right);
        result.insert({left, right});
    }
    return result;
}

bool has_edge(const EdgeSet& edges, int left, int right){
    if(left > right) std::swap(left, right);
    return edges.find({left, right}) != edges.end();
}

bool valid_certificate(
    int n,
    const std::vector<std::pair<int, int>>& input_edges,
    const ChordalGraphRecognitionResult& result
){
    const EdgeSet edges = make_edge_set(input_edges);
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(const auto& [left, right]: edges){
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }
    if(result.chordal){
        if(!result.induced_cycle.empty() ||
           result.perfect_elimination_order.size() !=
               static_cast<std::size_t>(n)) return false;
        std::vector<int> position(static_cast<std::size_t>(n), -1);
        for(int index = 0; index < n; ++index){
            const int vertex = result.perfect_elimination_order[
                static_cast<std::size_t>(index)
            ];
            if(vertex < 0 || n <= vertex ||
               position[static_cast<std::size_t>(vertex)] != -1) return false;
            position[static_cast<std::size_t>(vertex)] = index;
        }
        for(int vertex = 0; vertex < n; ++vertex){
            int first_later = -1;
            for(int to: graph[static_cast<std::size_t>(vertex)]){
                if(position[static_cast<std::size_t>(vertex)] <
                   position[static_cast<std::size_t>(to)] &&
                   (first_later == -1 ||
                    position[static_cast<std::size_t>(to)] <
                    position[static_cast<std::size_t>(first_later)])){
                    first_later = to;
                }
            }
            if(first_later == -1) continue;
            for(int to: graph[static_cast<std::size_t>(vertex)]){
                if(to != first_later &&
                   position[static_cast<std::size_t>(vertex)] <
                   position[static_cast<std::size_t>(to)] &&
                   !has_edge(edges, first_later, to)) return false;
            }
        }
        return true;
    }

    if(!result.perfect_elimination_order.empty() ||
       result.induced_cycle.size() < 4) return false;
    std::vector<unsigned char> in_cycle(static_cast<std::size_t>(n));
    for(int vertex: result.induced_cycle){
        if(vertex < 0 || n <= vertex ||
           in_cycle[static_cast<std::size_t>(vertex)]) return false;
        in_cycle[static_cast<std::size_t>(vertex)] = 1;
    }
    const std::size_t cycle_size = result.induced_cycle.size();
    for(std::size_t index = 0; index < cycle_size; ++index){
        if(!has_edge(
            edges, result.induced_cycle[index],
            result.induced_cycle[(index + 1) % cycle_size]
        )) return false;
    }
    std::size_t induced_edge_count = 0;
    for(const auto& [left, right]: edges){
        if(in_cycle[static_cast<std::size_t>(left)] &&
           in_cycle[static_cast<std::size_t>(right)]) ++induced_edge_count;
    }
    return induced_edge_count == cycle_size;
}

bool brute_chordal(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    std::vector<std::uint32_t> adjacent(static_cast<std::size_t>(n));
    for(const auto& [left, right]: make_edge_set(edges)){
        adjacent[static_cast<std::size_t>(left)] |=
            std::uint32_t(1) << right;
        adjacent[static_cast<std::size_t>(right)] |=
            std::uint32_t(1) << left;
    }
    const std::uint32_t full = n == 0 ? 0U :
        (std::uint32_t(1) << n) - 1U;
    std::vector<signed char> memo(
        static_cast<std::size_t>(std::uint32_t(1) << n), -1
    );
    std::function<bool(std::uint32_t)> solve = [&](std::uint32_t mask){
        signed char& cached = memo[static_cast<std::size_t>(mask)];
        if(cached != -1) return cached != 0;
        if(mask == 0){
            cached = 1;
            return true;
        }
        for(int vertex = 0; vertex < n; ++vertex){
            if((mask >> vertex & 1U) == 0) continue;
            const std::uint32_t neighbors =
                adjacent[static_cast<std::size_t>(vertex)] & mask;
            bool simplicial = true;
            for(int to = 0; to < n; ++to){
                if((neighbors >> to & 1U) == 0) continue;
                const std::uint32_t others = neighbors &
                    ~(std::uint32_t(1) << to);
                if((others & ~adjacent[static_cast<std::size_t>(to)]) != 0){
                    simplicial = false;
                    break;
                }
            }
            if(simplicial && solve(mask & ~(std::uint32_t(1) << vertex))){
                cached = 1;
                return true;
            }
        }
        cached = 0;
        return false;
    };
    return solve(full);
}

void check_case(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    bool compare_brute
){
    const auto result = chordal_graph_recognition(n, edges);
    assert(valid_certificate(n, edges, result));
    if(compare_brute) assert(result.chordal == brute_chordal(n, edges));
}

void test_random(){
    std::mt19937 rng(2026071602);
    for(int iteration = 0; iteration < 600; ++iteration){
        const int n = static_cast<int>(rng() % 11U);
        std::vector<std::pair<int, int>> edges;
        for(int left = 0; left < n; ++left){
            for(int right = left + 1; right < n; ++right){
                if(rng() % 100U < 38U) edges.push_back({left, right});
                if(rng() % 100U < 4U) edges.push_back({right, left});
            }
        }
        check_case(n, edges, true);
    }
}

void test_boundaries(){
    check_case(0, {}, true);
    check_case(4, {{0, 1}, {1, 2}, {2, 3}, {3, 0}}, true);
    check_case(4, {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {0, 2}}, true);

    constexpr int n = 30000;
    std::vector<std::pair<int, int>> path;
    path.reserve(static_cast<std::size_t>(n - 1));
    for(int vertex = 0; vertex + 1 < n; ++vertex){
        path.push_back({vertex, vertex + 1});
    }
    check_case(n, path, false);
    path.push_back({n - 1, 0});
    check_case(n, path, false);

    bool thrown = false;
    try{ (void)chordal_graph_recognition(3, {{0, 3}}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)chordal_graph_recognition(3, {{2, 2}}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    if(!(std::cin >> n >> m)){
        test_random();
        test_boundaries();
        return 0;
    }
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [left, right]: edges) std::cin >> left >> right;
    const auto result = chordal_graph_recognition(n, edges);
    if(!valid_certificate(n, edges, result)){
        throw std::runtime_error("invalid chordal graph certificate");
    }
    std::cout << (result.chordal ? "YES\n" : "NO\n");
}
