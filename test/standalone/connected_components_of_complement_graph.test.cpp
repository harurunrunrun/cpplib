// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/connectivity/connected_components_of_complement_graph.hpp"

std::vector<std::vector<int>> normalize_groups(std::vector<std::vector<int>> groups){
    for(auto& group: groups) std::sort(group.begin(), group.end());
    std::sort(groups.begin(), groups.end());
    return groups;
}

std::vector<std::vector<int>> naive_components(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<unsigned char>> adjacent(
        static_cast<std::size_t>(n),
        std::vector<unsigned char>(static_cast<std::size_t>(n), 0)
    );
    for(int vertex = 0; vertex < n; ++vertex){
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            adjacent[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(to)] = 1;
        }
    }
    std::vector<unsigned char> used(static_cast<std::size_t>(n), 0);
    std::vector<std::vector<int>> result;
    for(int start = 0; start < n; ++start){
        if(used[static_cast<std::size_t>(start)]) continue;
        result.push_back({});
        std::vector<int> stack = {start};
        used[static_cast<std::size_t>(start)] = 1;
        while(!stack.empty()){
            const int vertex = stack.back();
            stack.pop_back();
            result.back().push_back(vertex);
            for(int to = 0; to < n; ++to){
                if(to != vertex && !used[static_cast<std::size_t>(to)] &&
                   !adjacent[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(to)]){
                    used[static_cast<std::size_t>(to)] = 1;
                    stack.push_back(to);
                }
            }
        }
    }
    return normalize_groups(std::move(result));
}

void test_random(){
    std::mt19937 rng(2026071403);
    for(int iteration = 0; iteration < 600; ++iteration){
        const int n = 1 + static_cast<int>(rng() % 35);
        std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
        for(int left = 0; left < n; ++left){
            for(int right = left + 1; right < n; ++right){
                if(rng() % 100 < 55){
                    graph[static_cast<std::size_t>(left)].push_back(right);
                    graph[static_cast<std::size_t>(right)].push_back(left);
                }
            }
        }
        const auto result = connected_components_of_complement_graph(graph);
        assert(normalize_groups(result.groups) == naive_components(graph));
        for(int vertex = 0; vertex < n; ++vertex){
            const int id = result.component_id[static_cast<std::size_t>(vertex)];
            assert(id >= 0 && id < static_cast<int>(result.groups.size()));
            assert(std::find(result.groups[static_cast<std::size_t>(id)].begin(),
                             result.groups[static_cast<std::size_t>(id)].end(), vertex) !=
                   result.groups[static_cast<std::size_t>(id)].end());
        }
    }
}

void test_invalid_vertex(){
    bool thrown = false;
    try{
        (void)connected_components_of_complement_graph(
            std::vector<std::vector<int>>{{1}, {2}}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, edge_count;
    if(!(std::cin >> n >> edge_count)){
        test_random();
        test_invalid_vertex();
        return 0;
    }
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int edge = 0; edge < edge_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }
    auto groups = normalize_groups(
        connected_components_of_complement_graph(graph).groups
    );
    std::cout << groups.size() << '\n';
    for(const auto& group: groups){
        std::cout << group.size();
        for(int vertex: group) std::cout << ' ' << vertex;
        std::cout << '\n';
    }
}
