// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/chinese_postman.hpp"

std::optional<long long> brute_chinese_postman(
    int vertex_count,
    const std::vector<ChinesePostmanEdge<long long>>& edges
){
    constexpr long long infinity = std::numeric_limits<long long>::max() / 4;
    std::vector<std::vector<long long>> distance(
        static_cast<std::size_t>(vertex_count),
        std::vector<long long>(static_cast<std::size_t>(vertex_count), infinity)
    );
    std::vector<int> degree(static_cast<std::size_t>(vertex_count));
    long long base = 0;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        distance[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(vertex)] = 0;
    }
    for(const auto& edge: edges){
        base += edge.cost;
        ++degree[static_cast<std::size_t>(edge.from)];
        ++degree[static_cast<std::size_t>(edge.to)];
        auto& value = distance[static_cast<std::size_t>(edge.from)]
                              [static_cast<std::size_t>(edge.to)];
        value = std::min(value, edge.cost);
        distance[static_cast<std::size_t>(edge.to)]
                [static_cast<std::size_t>(edge.from)] = value;
    }
    for(int middle = 0; middle < vertex_count; ++middle){
        for(int from = 0; from < vertex_count; ++from){
            for(int to = 0; to < vertex_count; ++to){
                if(distance[static_cast<std::size_t>(from)][static_cast<std::size_t>(middle)] == infinity
                   || distance[static_cast<std::size_t>(middle)][static_cast<std::size_t>(to)] == infinity){
                    continue;
                }
                auto& value = distance[static_cast<std::size_t>(from)]
                                      [static_cast<std::size_t>(to)];
                value = std::min(
                    value,
                    distance[static_cast<std::size_t>(from)][static_cast<std::size_t>(middle)]
                    + distance[static_cast<std::size_t>(middle)][static_cast<std::size_t>(to)]
                );
            }
        }
    }
    int start = -1;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(degree[static_cast<std::size_t>(vertex)] != 0){ start = vertex; break; }
    }
    if(start != -1){
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(degree[static_cast<std::size_t>(vertex)] != 0
               && distance[static_cast<std::size_t>(start)][static_cast<std::size_t>(vertex)] == infinity){
                return std::nullopt;
            }
        }
    }
    std::vector<int> odd;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(degree[static_cast<std::size_t>(vertex)] & 1) odd.push_back(vertex);
    }
    std::vector<long long> dp(static_cast<std::size_t>(1 << odd.size()), infinity);
    dp[0] = 0;
    for(int mask = 0; mask < (1 << odd.size()); ++mask){
        if(dp[static_cast<std::size_t>(mask)] == infinity) continue;
        int first = 0;
        while(first < static_cast<int>(odd.size()) && (mask >> first & 1)) ++first;
        if(first == static_cast<int>(odd.size())) continue;
        for(int second = first + 1; second < static_cast<int>(odd.size()); ++second){
            if(mask >> second & 1) continue;
            const int next = mask | (1 << first) | (1 << second);
            dp[static_cast<std::size_t>(next)] = std::min(
                dp[static_cast<std::size_t>(next)],
                dp[static_cast<std::size_t>(mask)]
                + distance[static_cast<std::size_t>(odd[first])]
                          [static_cast<std::size_t>(odd[second])]
            );
        }
    }
    return base + dp.back();
}

void check_case(
    int vertex_count,
    const std::vector<ChinesePostmanEdge<long long>>& edges
){
    const auto expected = brute_chinese_postman(vertex_count, edges);
    const auto actual = undirected_chinese_postman<long long>(vertex_count, edges);
    assert(static_cast<bool>(actual) == static_cast<bool>(expected));
    if(actual) assert(actual->cost == *expected);
}

void self_test(){
    check_case(0, {});
    check_case(4, {});
    check_case(4, {{0, 1, 1}, {0, 2, 2}, {1, 3, 3}, {2, 3, 4}});
    check_case(2, {{0, 1, 1}, {0, 1, 2}, {0, 1, 3}});
    check_case(3, {{0, 0, 7}, {0, 1, 2}, {1, 2, 3}, {2, 0, 4}});
    assert(!undirected_chinese_postman<long long>(
        4, {{0, 1, 1}, {2, 3, 1}}
    ));

    bool thrown = false;
    try{
        (void)undirected_chinese_postman<long long>(2, {{0, 1, -1}});
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    std::mt19937 random(2026071312);
    for(int vertex_count = 1; vertex_count <= 9; ++vertex_count){
        for(int iteration = 0; iteration < 500; ++iteration){
            std::vector<ChinesePostmanEdge<long long>> edges;
            for(int vertex = 1; vertex < vertex_count; ++vertex){
                const int parent = static_cast<int>(random() % static_cast<unsigned>(vertex));
                edges.push_back({vertex, parent, 1LL + static_cast<long long>(random() % 30)});
            }
            for(int from = 0; from < vertex_count; ++from){
                for(int to = from + 1; to < vertex_count; ++to){
                    if(random() % 5 == 0){
                        edges.push_back({from, to, 1LL + static_cast<long long>(random() % 30)});
                    }
                }
            }
            check_case(vertex_count, edges);
        }
    }

    constexpr int size = 50;
    std::vector<ChinesePostmanEdge<long long>> star;
    for(int vertex = 1; vertex < size; ++vertex) star.push_back({0, vertex, 1});
    const auto result = undirected_chinese_postman<long long>(size, star);
    assert(result && result->cost == 2 * (size - 1));
}

int main(){
    int vertex_count, edge_count;
    if(!(std::cin >> vertex_count >> edge_count)){
        self_test();
        return 0;
    }
    std::vector<ChinesePostmanEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    for(int index = 0; index < edge_count; ++index){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        edges.push_back({from, to, cost});
    }
    const auto answer = undirected_chinese_postman<long long>(vertex_count, edges);
    if(answer) std::cout << answer->cost << '\n';
    else std::cout << "NONE\n";
}
