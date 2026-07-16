// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/treewidth_two_distance_oracle.hpp"

using Oracle = TreewidthTwoDistanceOracle<long long>;

std::vector<std::vector<long long>> brute_distances(
    int vertex_count,
    const std::vector<Oracle::Edge>& edges,
    long long infinity
){
    std::vector distances(
        static_cast<std::size_t>(vertex_count),
        std::vector<long long>(static_cast<std::size_t>(vertex_count), infinity)
    );
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        distances[static_cast<std::size_t>(vertex)]
            [static_cast<std::size_t>(vertex)] = 0;
    }
    for(const auto& edge: edges){
        auto& value = distances[static_cast<std::size_t>(edge.from)]
            [static_cast<std::size_t>(edge.to)];
        value = std::min(value, edge.weight);
    }
    for(int middle = 0; middle < vertex_count; ++middle){
        for(int first = 0; first < vertex_count; ++first){
            if(distances[static_cast<std::size_t>(first)]
               [static_cast<std::size_t>(middle)] == infinity){
                continue;
            }
            for(int second = 0; second < vertex_count; ++second){
                const long long suffix =
                    distances[static_cast<std::size_t>(middle)]
                    [static_cast<std::size_t>(second)];
                if(suffix == infinity) continue;
                auto& answer = distances[static_cast<std::size_t>(first)]
                    [static_cast<std::size_t>(second)];
                answer = std::min(
                    answer,
                    distances[static_cast<std::size_t>(first)]
                        [static_cast<std::size_t>(middle)] + suffix
                );
            }
        }
    }
    return distances;
}

void verify_case(
    int vertex_count,
    const std::vector<Oracle::Edge>& edges
){
    constexpr long long infinity = 1LL << 60;
    const Oracle oracle(vertex_count, edges, infinity);
    const auto expected = brute_distances(vertex_count, edges, infinity);
    assert(oracle.size() == vertex_count);
    assert(oracle.infinity() == infinity);
    for(int first = 0; first < vertex_count; ++first){
        for(int second = 0; second < vertex_count; ++second){
            const long long answer = oracle.distance(first, second);
            assert(answer == expected[static_cast<std::size_t>(first)]
                [static_cast<std::size_t>(second)]);
            assert(oracle.reachable(first, second) == (answer != infinity));
        }
    }
}

void self_test(){
    verify_case(0, {});
    verify_case(1, {{0, 0, 7}});
    verify_case(5, {
        {0, 1, 4}, {1, 0, 9}, {1, 2, 0}, {2, 3, 5},
        {3, 4, 2}, {4, 0, 8}, {0, 1, 3}, {2, 2, 0}
    });
    verify_case(6, {{0, 1, 2}, {2, 3, 4}, {4, 5, 6}});

    constexpr long long infinity = 1LL << 60;
    bool rejected = false;
    try{
        static_cast<void>(Oracle(-1, {}, infinity));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try{
        static_cast<void>(Oracle(2, {{0, 2, 1}}, infinity));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try{
        static_cast<void>(Oracle(2, {{0, 1, -1}}, infinity));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try{
        std::vector<Oracle::Edge> complete;
        for(int first = 0; first < 4; ++first){
            for(int second = first + 1; second < 4; ++second){
                complete.push_back({first, second, 1});
            }
        }
        static_cast<void>(Oracle(4, complete, infinity));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);

    std::mt19937_64 random(2026071701);
    for(int vertex_count = 2; vertex_count <= 28; ++vertex_count){
        for(int iteration = 0; iteration < 80; ++iteration){
            std::vector<std::pair<int, int>> host_edges{{0, 1}};
            std::vector<std::pair<int, int>> new_edges{{0, 1}};
            for(int vertex = 2; vertex < vertex_count; ++vertex){
                const auto [first, second] = host_edges[
                    static_cast<std::size_t>(random() % host_edges.size())
                ];
                host_edges.emplace_back(first, vertex);
                host_edges.emplace_back(second, vertex);
                new_edges.emplace_back(first, vertex);
                new_edges.emplace_back(second, vertex);
            }

            std::vector<Oracle::Edge> edges;
            for(const auto [first, second]: new_edges){
                if(random() % 5 == 0) continue;
                if(random() % 4 != 0){
                    edges.push_back({
                        first, second, static_cast<long long>(random() % 21)
                    });
                }
                if(random() % 4 != 0){
                    edges.push_back({
                        second, first, static_cast<long long>(random() % 21)
                    });
                }
                if(!edges.empty() && random() % 8 == 0){
                    edges.push_back(edges.back());
                    edges.back().weight += static_cast<long long>(random() % 4);
                }
            }
            if(random() % 2 == 0){
                const int vertex = static_cast<int>(random() %
                    static_cast<unsigned long long>(vertex_count));
                edges.push_back({vertex, vertex, 0});
            }
            verify_case(vertex_count, edges);
        }
    }

    constexpr int large_size = 20000;
    std::vector<Oracle::Edge> path;
    path.reserve(static_cast<std::size_t>(2 * large_size - 2));
    for(int vertex = 0; vertex + 1 < large_size; ++vertex){
        path.push_back({vertex, vertex + 1, 1});
        path.push_back({vertex + 1, vertex, 2});
    }
    const Oracle large(large_size, path, infinity);
    assert(large.hierarchy_depth() <= 32);
    assert(large.distance(0, large_size - 1) == large_size - 1);
    assert(large.distance(large_size - 1, 0) == 2LL * (large_size - 1));
}

int main(){
    int vertex_count;
    int edge_count;
    int query_count;
    if(!(std::cin >> vertex_count >> edge_count >> query_count)){
        self_test();
        return 0;
    }
    std::vector<Oracle::Edge> edges(static_cast<std::size_t>(edge_count));
    for(auto& edge: edges){
        std::cin >> edge.from >> edge.to >> edge.weight;
    }
    constexpr long long infinity = 1LL << 60;
    const Oracle oracle(vertex_count, edges, infinity);
    for(int query = 0; query < query_count; ++query){
        int from;
        int to;
        std::cin >> from >> to;
        const long long answer = oracle.distance(from, to);
        std::cout << (answer == infinity ? -1 : answer) << '\n';
    }
}
