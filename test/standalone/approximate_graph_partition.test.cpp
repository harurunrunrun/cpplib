// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/fiduccia_mattheyses.hpp"
#include "../../src/approximate/graph/graph_partition.hpp"
#include "../../src/approximate/graph/kernighan_lin.hpp"
#include "../../src/approximate/graph/multilevel_partitioning.hpp"
#include "../../src/approximate/graph/spectral_bisection.hpp"

namespace {

using approximate::graph::BipartitionResult;
using approximate::graph::WeightedUndirectedEdge;

void require(bool condition){
    if(!condition) throw std::runtime_error("graph partition test failed");
}

bool close(long double left, long double right){
    const long double scale = std::max({1.0L, std::abs(left), std::abs(right)});
    return std::abs(left - right) <= 1.0e-11L * scale;
}

std::size_t imbalance(const std::vector<unsigned char>& side){
    std::size_t zeros = 0;
    for(const unsigned char value : side) zeros += value == 0 ? 1U : 0U;
    const std::size_t ones = side.size() - zeros;
    return zeros > ones ? zeros - ones : ones - zeros;
}

void check_result(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    const BipartitionResult& result,
    std::size_t maximum_imbalance
){
    require(result.side.size() == vertex_count);
    for(const unsigned char value : result.side) require(value <= 1);
    require(imbalance(result.side) <= maximum_imbalance);
    require(close(result.cut_weight, approximate::graph::weighted_cut_weight(vertex_count, edges, result.side)));
}

template<class Exception, class Function>
void require_throws(Function&& function){
    bool thrown = false;
    try{
        std::forward<Function>(function)();
    }catch(const Exception&){
        thrown = true;
    }
    require(thrown);
}

void boundary_tests(){
    using namespace approximate::graph;
    const long double maximum = std::numeric_limits<long double>::max();
    require_throws<std::invalid_argument>([]{
        static_cast<void>(weighted_cut_weight(2, {{0, 1, -1.0L}}, {0, 1}));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(weighted_cut_weight(
            2, {{0, 1, std::numeric_limits<long double>::infinity()}}, {0, 1}
        ));
    });
    require_throws<std::out_of_range>([]{
        static_cast<void>(weighted_cut_weight(2, {{0, 2, 1.0L}}, {0, 1}));
    });
    require_throws<std::overflow_error>([maximum]{
        static_cast<void>(weighted_cut_weight(
            2, {{0, 1, maximum * 0.75L}, {1, 0, maximum * 0.75L}}, {0, 1}
        ));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(weighted_cut_weight(2, {}, {0, 2}));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(fiduccia_mattheyses(3, {}, 0, 1));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(spectral_bisection(2, {{0, 1, 1.0L}}, 10, -1.0L));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(multilevel_graph_partitioning(2, {}, 1));
    });
    require(close(weighted_cut_weight(
        2, {{0, 0, maximum}, {0, 1, 2.0L}, {1, 0, 3.0L}}, {0, 1}
    ), 5.0L));
    const std::vector<WeightedUndirectedEdge> large_weights{
        {0, 1, maximum / 4.0L}, {1, 2, maximum / 8.0L}, {2, 3, maximum / 8.0L}
    };
    require(std::isfinite(kernighan_lin(4, large_weights).cut_weight));
    require(std::isfinite(fiduccia_mattheyses(4, large_weights).cut_weight));
    require(std::isfinite(spectral_bisection(4, large_weights).cut_weight));
    require(std::isfinite(multilevel_graph_partitioning(4, large_weights).cut_weight));
    require(kernighan_lin(0, {}).side.empty());
    require(fiduccia_mattheyses(0, {}).side.empty());
    require(spectral_bisection(0, {}).side.empty());
    require(multilevel_graph_partitioning(0, {}).side.empty());
    require(multilevel_graph_partitioning(
        2, {{0, 1, 1.0L}}, 2, 0, std::numeric_limits<std::size_t>::max()
    ).side.size() == 2);
    require(imbalance(multilevel_graph_partitioning(
        10,
        {{0, 1, 2.0L}, {1, 2, 2.0L}, {2, 3, 2.0L}, {3, 4, 2.0L}, {4, 5, 2.0L},
         {5, 6, 2.0L}, {6, 7, 2.0L}, {7, 8, 2.0L}, {8, 9, 2.0L}, {9, 0, 2.0L}},
        2,
        0
    ).side) == 0);

    const std::vector<unsigned char> unbalanced{0, 0, 0, 1};
    const auto preserved = kernighan_lin(4, {{0, 3, 1.0L}}, unbalanced, 3);
    require(imbalance(preserved.side) == imbalance(unbalanced));
}

}  // namespace

int main(){
    using namespace approximate::graph;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t case_count = 0;
    std::cin >> case_count;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t identifier = 0;
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::size_t maximum_imbalance = 0;
        long double upper_bound = -1.0L;
        std::cin >> identifier >> vertex_count >> edge_count >> maximum_imbalance >> upper_bound;
        std::vector<WeightedUndirectedEdge> edges(edge_count);
        for(auto& edge : edges) std::cin >> edge.left >> edge.right >> edge.weight;

        const auto initial = balanced_bipartition(vertex_count);
        const long double initial_cut = weighted_cut_weight(vertex_count, edges, initial);
        const BipartitionResult kl = kernighan_lin(vertex_count, edges);
        const BipartitionResult fm = fiduccia_mattheyses(
            vertex_count, edges, initial, maximum_imbalance, 20
        );
        const BipartitionResult spectral = spectral_bisection(vertex_count, edges, 300, 1.0e-13L);
        const MultilevelPartitionResult multilevel = multilevel_graph_partitioning(
            vertex_count, edges, 6, maximum_imbalance, 16, 4
        );
        check_result(vertex_count, edges, kl, vertex_count % 2);
        check_result(vertex_count, edges, fm, maximum_imbalance);
        check_result(vertex_count, edges, spectral, vertex_count % 2);
        check_result(vertex_count, edges, {multilevel.side, multilevel.cut_weight}, maximum_imbalance);
        require(kl.cut_weight <= initial_cut || close(kl.cut_weight, initial_cut));
        require(fm.cut_weight <= initial_cut || close(fm.cut_weight, initial_cut));
        if(upper_bound >= 0.0L){
            require(kl.cut_weight <= upper_bound || close(kl.cut_weight, upper_bound));
            require(fm.cut_weight <= upper_bound || close(fm.cut_weight, upper_bound));
            require(spectral.cut_weight <= upper_bound || close(spectral.cut_weight, upper_bound));
            require(multilevel.cut_weight <= upper_bound || close(multilevel.cut_weight, upper_bound));
        }
        require(kernighan_lin(vertex_count, edges).side == kl.side);
        require(fiduccia_mattheyses(vertex_count, edges, initial, maximum_imbalance, 20).side == fm.side);
        require(spectral_bisection(vertex_count, edges, 300, 1.0e-13L).side == spectral.side);
        require(multilevel_graph_partitioning(vertex_count, edges, 6, maximum_imbalance, 16, 4).side
            == multilevel.side);
        std::cout << "OK " << identifier << '\n';
    }
}
