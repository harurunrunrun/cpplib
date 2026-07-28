// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/decomposition/ear_decomposition.hpp"
#include "../../src/algorithm/graph/specialized/subgraph_analysis/minimum_cycle_basis.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        int vertex_count;
        int edge_count;
        std::cin >> kind >> vertex_count >> edge_count;
        if(kind == 0){
            std::vector<MinimumCycleBasisEdge> edges(
                static_cast<std::size_t>(edge_count)
            );
            for(auto& edge: edges){
                std::cin >> edge.from >> edge.to >> edge.weight;
            }
            MinimumCycleBasisResult result =
                minimum_cycle_basis(vertex_count, edges);
            const std::size_t word_count =
                (static_cast<std::size_t>(edge_count) + 63U) / 64U;
            std::vector<std::vector<std::uint64_t>> basis(
                static_cast<std::size_t>(edge_count)
            );
            long long total_weight = 0;
            int rank = 0;
            for(const auto& cycle: result.cycles){
                std::vector<int> degree(
                    static_cast<std::size_t>(vertex_count), 0
                );
                std::vector<std::uint64_t> bits(word_count, 0);
                for(int edge_id: cycle){
                    const auto& edge =
                        edges[static_cast<std::size_t>(edge_id)];
                    ++degree[static_cast<std::size_t>(edge.from)];
                    ++degree[static_cast<std::size_t>(edge.to)];
                    bits[static_cast<std::size_t>(edge_id) >> 6U] ^=
                        std::uint64_t{1}
                            << (static_cast<unsigned>(edge_id) & 63U);
                    total_weight += edge.weight;
                }
                for(int value: degree) if((value & 1) != 0) return 2;
                int pivot = -1;
                for(int edge_id = edge_count; edge_id-- > 0; ){
                    if(((bits[static_cast<std::size_t>(edge_id) >> 6U]
                        >> (static_cast<unsigned>(edge_id) & 63U))
                        & 1U) == 0){
                        continue;
                    }
                    if(basis[static_cast<std::size_t>(edge_id)].empty()){
                        pivot = edge_id;
                        break;
                    }
                    for(std::size_t word = 0; word < word_count; ++word){
                        bits[word] ^=
                            basis[static_cast<std::size_t>(edge_id)][word];
                    }
                }
                if(pivot == -1) return 3;
                basis[static_cast<std::size_t>(pivot)] = std::move(bits);
                ++rank;
            }
            if(total_weight != result.weight) return 4;
            std::cout << result.weight << ' ' << rank << '\n';
            continue;
        }
        std::vector<std::pair<int, int>> edges(
            static_cast<std::size_t>(edge_count)
        );
        for(auto& [left, right]: edges) std::cin >> left >> right;
        auto result = ear_decomposition(vertex_count, edges);
        if(result){
            if(result->ears.empty()
                || result->ears.size() != result->ear_edge_ids.size()){
                return 5;
            }
            std::vector<unsigned char> included(
                static_cast<std::size_t>(vertex_count), 0
            );
            std::vector<int> edge_use(
                static_cast<std::size_t>(edge_count), 0
            );
            for(std::size_t ear_index = 0;
                ear_index < result->ears.size();
                ++ear_index){
                const auto& vertices = result->ears[ear_index];
                const auto& edge_ids = result->ear_edge_ids[ear_index];
                if(ear_index == 0){
                    if(vertices.size() != edge_ids.size()) return 6;
                }else{
                    if(vertices.size() != edge_ids.size() + 1
                        || included[static_cast<std::size_t>(
                            vertices.front()
                        )] == 0
                        || included[static_cast<std::size_t>(
                            vertices.back()
                        )] == 0){
                        return 7;
                    }
                    for(std::size_t index = 1;
                        index + 1 < vertices.size();
                        ++index){
                        if(included[static_cast<std::size_t>(
                            vertices[index]
                        )] != 0){
                            return 8;
                        }
                    }
                }
                for(std::size_t index = 0; index < edge_ids.size(); ++index){
                    int edge_id = edge_ids[index];
                    if(++edge_use[static_cast<std::size_t>(edge_id)] != 1){
                        return 9;
                    }
                    int left = vertices[index];
                    int right = ear_index == 0
                        ? vertices[(index + 1) % vertices.size()]
                        : vertices[index + 1];
                    auto [u, v] = edges[static_cast<std::size_t>(edge_id)];
                    if(!((u == left && v == right)
                        || (u == right && v == left))){
                        return 10;
                    }
                }
                for(int vertex: vertices){
                    included[static_cast<std::size_t>(vertex)] = 1;
                }
            }
            for(int value: edge_use) if(value != 1) return 11;
        }
        std::cout << static_cast<int>(result.has_value()) << '\n';
    }
}
