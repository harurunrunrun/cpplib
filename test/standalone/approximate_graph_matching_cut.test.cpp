// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <random>
#include <tuple>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/matching.hpp"
#include "../../src/approximate/graph/max_cut.hpp"

namespace {

void print_vector(const std::vector<std::size_t>& values){
    std::cout << values.size();
    for(const std::size_t value : values) std::cout << ' ' << value;
}

void print_sides(const std::vector<unsigned char>& values){
    std::cout << values.size();
    for(const unsigned char value : values) std::cout << ' ' << static_cast<unsigned int>(value);
}

}  // namespace

int main(){
    using namespace approximate::graph;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count = 0;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::cin >> vertex_count >> edge_count;
        std::vector<std::pair<std::size_t, std::size_t>> edges(edge_count);
        std::vector<std::tuple<std::size_t, std::size_t, long long>> weighted;
        weighted.reserve(edge_count);
        for(auto& edge : edges){
            long long weight = 0;
            std::cin >> edge.first >> edge.second >> weight;
            weighted.emplace_back(edge.first, edge.second, weight);
        }
        const MatchingResult unweighted = greedy_matching(vertex_count, edges);
        const MatchingResult by_weight = greedy_weighted_matching(vertex_count, weighted);
        const MaxCutResult cut = greedy_max_cut(vertex_count, edges);
        std::mt19937_64 generator(123456789ULL);
        const MaxCutResult random = random_cut(vertex_count, edges, generator);
        if(random.cut_edges != cut_edge_count(vertex_count, edges, random.side)) return 2;
        print_vector(unweighted.edge_indices);
        std::cout << " | ";
        print_vector(by_weight.edge_indices);
        std::cout << " | ";
        print_sides(cut.side);
        std::cout << ' ' << cut.cut_edges << '\n';
    }
}
