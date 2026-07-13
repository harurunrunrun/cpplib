// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/randomized/randomized_min_cut.hpp"

namespace {

std::size_t represented_cut(
    const std::vector<approximate::randomized::UndirectedEdge>& edges,
    const std::vector<std::uint8_t>& side
) {
    std::size_t cut = 0;
    for(const auto& edge: edges){
        if(side[edge.first] != side[edge.second]) ++cut;
    }
    return cut;
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    if(!(std::cin >> case_count)) return 0;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t vertex_count;
        std::size_t edge_count;
        std::size_t expected;
        int contraction_always_optimal;
        std::cin >> vertex_count >> edge_count >> expected >> contraction_always_optimal;
        std::vector<approximate::randomized::UndirectedEdge> edges(edge_count);
        for(auto& edge: edges) std::cin >> edge.first >> edge.second;

        std::mt19937_64 first_random(810000ULL + static_cast<unsigned long long>(case_index));
        std::mt19937_64 second_random(810000ULL + static_cast<unsigned long long>(case_index));
        const auto first = approximate::randomized::karger_min_cut(
            vertex_count, edges, 48, first_random
        );
        const auto repeated = approximate::randomized::karger_min_cut(
            vertex_count, edges, 48, second_random
        );
        assert(first.cut_size == repeated.cut_size);
        assert(first.side == repeated.side);
        assert(first.side.size() == vertex_count);
        assert(represented_cut(edges, first.side) == first.cut_size);
        assert(first.cut_size >= expected);
        if(contraction_always_optimal != 0) assert(first.cut_size == expected);

        std::mt19937_64 third_random(910000ULL + static_cast<unsigned long long>(case_index));
        const auto stein = approximate::randomized::karger_stein_min_cut(
            vertex_count, edges, 3, third_random
        );
        assert(stein.side.size() == vertex_count);
        assert(represented_cut(edges, stein.side) == stein.cut_size);
        assert(stein.cut_size == expected);
        std::cout << stein.cut_size << '\n';
    }

    bool endpoint_thrown = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::randomized::karger_min_cut(
            2, {{0, 2}}, 1, random
        ));
    }catch(const std::out_of_range&){
        endpoint_thrown = true;
    }
    assert(endpoint_thrown);

    bool trials_thrown = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::randomized::karger_min_cut(
            2, {{0, 1}}, 0, random
        ));
    }catch(const std::invalid_argument&){
        trials_thrown = true;
    }
    assert(trials_thrown);
}
