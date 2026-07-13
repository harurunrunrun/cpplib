// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/counting_c4.hpp"

std::vector<long long> brute_count_c4(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    const int m = static_cast<int>(edges.size());
    std::vector<long long> answer(static_cast<std::size_t>(m));
    for(int first = 0; first < m; ++first){
        for(int second = first + 1; second < m; ++second){
            for(int third = second + 1; third < m; ++third){
                for(int fourth = third + 1; fourth < m; ++fourth){
                    const std::array<int, 4> chosen{
                        first, second, third, fourth
                    };
                    std::vector<int> degree(static_cast<std::size_t>(n));
                    std::vector<int> parent(static_cast<std::size_t>(n));
                    for(int vertex = 0; vertex < n; ++vertex){
                        parent[static_cast<std::size_t>(vertex)] = vertex;
                    }
                    const auto find = [&](auto&& self, int vertex) -> int{
                        int& p = parent[static_cast<std::size_t>(vertex)];
                        return p == vertex ? vertex : p = self(self, p);
                    };
                    for(int edge: chosen){
                        const auto [left, right] =
                            edges[static_cast<std::size_t>(edge)];
                        ++degree[static_cast<std::size_t>(left)];
                        ++degree[static_cast<std::size_t>(right)];
                        const int left_root = find(find, left);
                        const int right_root = find(find, right);
                        parent[static_cast<std::size_t>(left_root)] = right_root;
                    }
                    int used_vertices = 0;
                    int component = -1;
                    bool valid = true;
                    for(int vertex = 0; vertex < n; ++vertex){
                        const int value = degree[static_cast<std::size_t>(vertex)];
                        if(value == 0) continue;
                        ++used_vertices;
                        if(value != 2) valid = false;
                        const int root = find(find, vertex);
                        if(component == -1) component = root;
                        if(root != component) valid = false;
                    }
                    if(!valid || used_vertices != 4) continue;
                    for(int edge: chosen){
                        ++answer[static_cast<std::size_t>(edge)];
                    }
                }
            }
        }
    }
    return answer;
}

void check_case(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    bool compare_brute
){
    const auto actual = count_c4_per_edge(n, edges);
    if(compare_brute) assert(actual == brute_count_c4(n, edges));
    __int128_t sum = 0;
    for(long long value: actual) sum += value;
    assert(count_c4(n, edges) == sum / 4);
}

void test_random(){
    std::mt19937 rng(2026071603);
    for(int iteration = 0; iteration < 1000; ++iteration){
        const int n = 2 + static_cast<int>(rng() % 7U);
        const int m = static_cast<int>(rng() % 14U);
        std::vector<std::pair<int, int>> edges;
        for(int edge = 0; edge < m; ++edge){
            int left = static_cast<int>(rng() % static_cast<unsigned int>(n));
            int right = static_cast<int>(rng() % static_cast<unsigned int>(n - 1));
            if(right >= left) ++right;
            edges.push_back({left, right});
        }
        check_case(n, edges, true);
    }
}

void test_boundaries(){
    check_case(0, {}, true);
    check_case(4, {{0, 1}, {1, 2}, {2, 3}, {3, 0}}, true);
    check_case(
        4,
        {
            {0, 1}, {0, 1},
            {1, 2}, {1, 2}, {1, 2},
            {2, 3}, {2, 3}, {2, 3}, {2, 3},
            {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}
        },
        true
    );

    constexpr int right_count = 5000;
    std::vector<std::pair<int, int>> complete_bipartite;
    complete_bipartite.reserve(static_cast<std::size_t>(2 * right_count));
    for(int vertex = 0; vertex < right_count; ++vertex){
        complete_bipartite.push_back({0, vertex + 2});
        complete_bipartite.push_back({1, vertex + 2});
    }
    const auto answer = count_c4_per_edge(
        right_count + 2, complete_bipartite
    );
    for(long long value: answer) assert(value == right_count - 1LL);
    assert(count_c4(right_count + 2, complete_bipartite) ==
        right_count * (right_count - 1LL) / 2);

    constexpr long long side_multiplicity = 60000;
    std::vector<std::pair<int, int>> parallel_square;
    parallel_square.reserve(
        static_cast<std::size_t>(4 * side_multiplicity)
    );
    for(long long copy = 0; copy < side_multiplicity; ++copy){
        parallel_square.push_back({0, 1});
        parallel_square.push_back({1, 2});
        parallel_square.push_back({2, 3});
        parallel_square.push_back({3, 0});
    }
    const auto parallel_answer = count_c4_per_edge(4, parallel_square);
    const long long per_edge = side_multiplicity * side_multiplicity *
        side_multiplicity;
    for(long long value: parallel_answer) assert(value == per_edge);
    const __int128_t total = static_cast<__int128_t>(side_multiplicity) *
        side_multiplicity * side_multiplicity * side_multiplicity;
    assert(count_c4(4, parallel_square) == total);

    bool thrown = false;
    try{ (void)count_c4_per_edge(3, {{0, 3}}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)count_c4_per_edge(3, {{1, 1}}); }
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
    const auto answer = count_c4_per_edge(n, edges);
    for(int edge = 0; edge < m; ++edge){
        if(edge != 0) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(edge)];
    }
    std::cout << '\n';
}
