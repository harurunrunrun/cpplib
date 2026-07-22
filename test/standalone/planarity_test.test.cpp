// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>
#include <random>

#include "../../src/algorithm/graph/specialized/planarity_test.hpp"

bool rotation_is_planar(
    const std::vector<std::vector<int>>& order,
    int edge_count
){
    const int n = static_cast<int>(order.size());
    std::vector<std::vector<unsigned char>> used(
        static_cast<std::size_t>(n),
        std::vector<unsigned char>(static_cast<std::size_t>(n))
    );
    int faces = 0;
    for(int first = 0; first < n; ++first){
        for(const int second: order[static_cast<std::size_t>(first)]){
            if(used[static_cast<std::size_t>(first)][static_cast<std::size_t>(second)]){
                continue;
            }
            ++faces;
            int from = first;
            int to = second;
            do{
                used[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)] = 1;
                const auto& around = order[static_cast<std::size_t>(to)];
                const auto found = std::find(around.begin(), around.end(), from);
                assert(found != around.end());
                const std::size_t position = static_cast<std::size_t>(found - around.begin());
                const int next = around[(position + around.size() - 1) % around.size()];
                from = to;
                to = next;
            }while(from != first || to != second);
        }
    }
    return n - edge_count + faces == 2;
}

bool enumerate_rotations(
    std::vector<std::vector<int>>& order,
    int vertex,
    int edge_count
){
    const int n = static_cast<int>(order.size());
    if(vertex == n) return rotation_is_planar(order, edge_count);
    auto& neighbors = order[static_cast<std::size_t>(vertex)];
    if(neighbors.size() <= 2){
        return enumerate_rotations(order, vertex + 1, edge_count);
    }
    std::sort(neighbors.begin() + 1, neighbors.end());
    do{
        if(enumerate_rotations(order, vertex + 1, edge_count)) return true;
    }while(std::next_permutation(neighbors.begin() + 1, neighbors.end()));
    return false;
}

bool brute_connected_planarity(
    int n,
    const std::vector<std::pair<int,int>>& edges
){
    if(n <= 1) return true;
    std::vector<std::vector<int>> order(static_cast<std::size_t>(n));
    for(const auto [first, second]: edges){
        order[static_cast<std::size_t>(first)].push_back(second);
        order[static_cast<std::size_t>(second)].push_back(first);
    }
    return enumerate_rotations(order, 0, static_cast<int>(edges.size()));
}

void self_test(){
    assert(is_planar_graph(0, {}));
    assert(is_planar_graph(4, {{0,1},{1,2},{2,3}}));
    assert(is_planar_graph(3, {{0,0},{0,1},{0,1},{1,2},{2,0}}));

    std::vector<std::pair<int,int>> k5;
    for(int first = 0; first < 5; ++first){
        for(int second = first + 1; second < 5; ++second){
            k5.emplace_back(first, second);
        }
    }
    assert(!is_planar_graph(5, k5));
    std::vector<std::pair<int,int>> k33;
    for(int first = 0; first < 3; ++first){
        for(int second = 3; second < 6; ++second){
            k33.emplace_back(first, second);
        }
    }
    assert(!is_planar_graph(6, k33));

    for(unsigned mask = 0; mask < (1U << 10); ++mask){
        std::vector<std::pair<int,int>> edges;
        unsigned bit = 0;
        for(int first = 0; first < 5; ++first){
            for(int second = first + 1; second < 5; ++second, ++bit){
                if((mask >> bit) & 1U) edges.emplace_back(first, second);
            }
        }
        std::vector<int> parent(5);
        std::iota(parent.begin(), parent.end(), 0);
        const auto leader = [&](int vertex, const auto& self) -> int {
            return parent[static_cast<std::size_t>(vertex)] == vertex ? vertex :
                parent[static_cast<std::size_t>(vertex)] =
                    self(parent[static_cast<std::size_t>(vertex)], self);
        };
        for(const auto [first, second]: edges){
            parent[static_cast<std::size_t>(leader(first, leader))] = leader(second, leader);
        }
        bool connected = true;
        for(int vertex = 1; vertex < 5; ++vertex){
            connected = connected && leader(vertex, leader) == leader(0, leader);
        }
        if(connected){
            assert(is_planar_graph(5, edges) == brute_connected_planarity(5, edges));
        }
    }

    constexpr int rows = 300;
    constexpr int columns = 300;
    std::vector<std::pair<int,int>> grid;
    for(int row = 0; row < rows; ++row){
        for(int column = 0; column < columns; ++column){
            const int vertex = row * columns + column;
            if(row + 1 < rows) grid.emplace_back(vertex, vertex + columns);
            if(column + 1 < columns) grid.emplace_back(vertex, vertex + 1);
        }
    }
    assert(is_planar_graph(rows * columns, grid));
    std::mt19937_64 random(2026071620);
    for(int iteration = 0; iteration < 500; ++iteration){
        constexpr int n = 6;
        std::vector<std::pair<int, int>> edges;
        std::vector<std::vector<unsigned char>> present(
            n, std::vector<unsigned char>(n)
        );
        for(int vertex = 1; vertex < n; ++vertex){
            const int parent_vertex = static_cast<int>(
                random() % static_cast<unsigned long long>(vertex)
            );
            edges.emplace_back(parent_vertex, vertex);
            present[static_cast<std::size_t>(parent_vertex)]
                [static_cast<std::size_t>(vertex)] = 1;
            present[static_cast<std::size_t>(vertex)]
                [static_cast<std::size_t>(parent_vertex)] = 1;
        }
        while(edges.size() < 9 && random() % 2 == 0){
            const int first = static_cast<int>(random() % n);
            const int second = static_cast<int>(random() % n);
            if(first == second || present[static_cast<std::size_t>(first)]
                [static_cast<std::size_t>(second)]) continue;
            edges.emplace_back(first, second);
            present[static_cast<std::size_t>(first)][static_cast<std::size_t>(second)] = 1;
            present[static_cast<std::size_t>(second)][static_cast<std::size_t>(first)] = 1;
        }
        assert(is_planar_graph(n, edges) == brute_connected_planarity(n, edges));

    }
}
int main(){
    int vertex_count;
    int edge_count;
    if(!(std::cin >> vertex_count >> edge_count)){
        self_test();
        return 0;
    }
    std::vector<std::pair<int,int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [first, second]: edges) std::cin >> first >> second;
    std::cout << static_cast<int>(is_planar_graph(vertex_count, edges)) << '\n';
}
