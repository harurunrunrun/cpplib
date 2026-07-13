// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/three_edge_connected_components.hpp"

bool connected_without(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    int source,
    int sink,
    int removed_first,
    int removed_second
){
    if(source == sink) return true;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int edge = 0; edge < static_cast<int>(edges.size()); ++edge){
        if(edge == removed_first || edge == removed_second) continue;
        const auto [left, right] = edges[static_cast<std::size_t>(edge)];
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }
    std::vector<unsigned char> reached(static_cast<std::size_t>(n));
    std::vector<int> stack{source};
    reached[static_cast<std::size_t>(source)] = 1;
    while(!stack.empty()){
        const int vertex = stack.back();
        stack.pop_back();
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(reached[static_cast<std::size_t>(to)]) continue;
            reached[static_cast<std::size_t>(to)] = 1;
            stack.push_back(to);
        }
    }
    return reached[static_cast<std::size_t>(sink)] != 0;
}

bool brute_same_component(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    int left,
    int right
){
    const int m = static_cast<int>(edges.size());
    if(!connected_without(n, edges, left, right, -1, -1)) return false;
    for(int first = 0; first < m; ++first){
        if(!connected_without(n, edges, left, right, first, -1)){
            return false;
        }
        for(int second = first + 1; second < m; ++second){
            if(!connected_without(n, edges, left, right, first, second)){
                return false;
            }
        }
    }
    return true;
}

bool valid_partition(
    int n,
    const ThreeEdgeConnectedComponentsResult& result
){
    if(result.count != static_cast<int>(result.groups.size()) ||
       result.id.size() != static_cast<std::size_t>(n)) return false;
    std::vector<int> seen(static_cast<std::size_t>(n), -1);
    for(int component = 0; component < result.count; ++component){
        if(result.groups[static_cast<std::size_t>(component)].empty()){
            return false;
        }
        for(int vertex: result.groups[static_cast<std::size_t>(component)]){
            if(vertex < 0 || n <= vertex ||
               seen[static_cast<std::size_t>(vertex)] != -1) return false;
            seen[static_cast<std::size_t>(vertex)] = component;
            if(result.id[static_cast<std::size_t>(vertex)] != component){
                return false;
            }
        }
    }
    for(int component: seen) if(component == -1) return false;
    return true;
}

void check_case(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    bool compare_naive
){
    const auto result = three_edge_connected_components(n, edges);
    assert(valid_partition(n, result));
    if(!compare_naive) return;
    for(int left = 0; left < n; ++left){
        for(int right = 0; right < n; ++right){
            assert((result.id[static_cast<std::size_t>(left)] ==
                    result.id[static_cast<std::size_t>(right)]) ==
                brute_same_component(n, edges, left, right));
        }
    }
}

std::vector<std::vector<int>> canonical_groups(
    ThreeEdgeConnectedComponentsResult result
){
    for(auto& group: result.groups){
        std::sort(group.begin(), group.end());
    }
    std::sort(result.groups.begin(), result.groups.end());
    return result.groups;
}

void test_random(){
    std::mt19937 rng(2026071702);
    for(int iteration = 0; iteration < 700; ++iteration){
        const int n = 1 + static_cast<int>(rng() % 8U);
        const int m = static_cast<int>(rng() % 13U);
        std::vector<std::pair<int, int>> edges;
        for(int edge = 0; edge < m; ++edge){
            edges.push_back({
                static_cast<int>(rng() % static_cast<unsigned int>(n)),
                static_cast<int>(rng() % static_cast<unsigned int>(n))
            });
        }
        check_case(n, edges, true);
    }
}

void test_boundaries(){
    check_case(0, {}, true);
    check_case(1, {{0, 0}, {0, 0}}, true);
    check_case(2, {{0, 1}, {0, 1}}, true);
    check_case(2, {{0, 1}, {0, 1}, {0, 1}}, true);
    check_case(
        4,
        {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}},
        true
    );

    constexpr int cycle_size = 200000;
    std::vector<std::pair<int, int>> cycle;
    cycle.reserve(static_cast<std::size_t>(cycle_size));
    for(int vertex = 0; vertex + 1 < cycle_size; ++vertex){
        cycle.push_back({vertex, vertex + 1});
    }
    cycle.push_back({cycle_size - 1, 0});
    const auto cycle_result = three_edge_connected_components(
        cycle_size, cycle
    );
    assert(cycle_result.count == cycle_size);

    constexpr int path_size = 66667;
    std::vector<std::pair<int, int>> triple_path;
    triple_path.reserve(static_cast<std::size_t>(3 * (path_size - 1)));
    for(int vertex = 0; vertex + 1 < path_size; ++vertex){
        for(int copy = 0; copy < 3; ++copy){
            triple_path.push_back({vertex, vertex + 1});
        }
    }
    const auto path_result = three_edge_connected_components(
        path_size, triple_path
    );
    assert(path_result.count == 1);
    assert(path_result.groups.front().size() ==
        static_cast<std::size_t>(path_size));

    bool thrown = false;
    try{ (void)three_edge_connected_components(3, {{0, 3}}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)three_edge_connected_components(-1, {}); }
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
    const auto groups = canonical_groups(
        three_edge_connected_components(n, edges)
    );
    std::cout << groups.size() << '\n';
    for(const auto& group: groups){
        std::cout << group.size();
        for(int vertex: group) std::cout << ' ' << vertex;
        std::cout << '\n';
    }
}
