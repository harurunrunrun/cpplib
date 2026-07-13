// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/markov_clustering.hpp"

namespace {

using approximate::graph::MarkovClusteringResult;
using approximate::graph::WeightedUndirectedEdge;

void require(bool condition){
    if(!condition) throw std::runtime_error("Markov clustering test failed");
}

bool close(long double left, long double right){
    const long double scale = std::max({1.0L, std::abs(left), std::abs(right)});
    return std::abs(left - right) <= 1.0e-9L * scale;
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

void check_result(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    const MarkovClusteringResult& result,
    std::size_t maximum_iterations
){
    require(result.labels.size() == vertex_count);
    require(result.transition_matrix.size() == vertex_count);
    require(result.iterations <= maximum_iterations);
    std::vector<bool> seen(result.community_count, false);
    std::size_t next_label = 0;
    for(const std::size_t label : result.labels){
        require(label < result.community_count);
        if(!seen[label]){
            require(label == next_label);
            seen[label] = true;
            ++next_label;
        }
    }
    require(next_label == result.community_count);
    for(const auto& row : result.transition_matrix) require(row.size() == vertex_count);
    for(std::size_t column = 0; column < vertex_count; ++column){
        long double sum = 0.0L;
        for(std::size_t row = 0; row < vertex_count; ++row){
            const long double value = result.transition_matrix[row][column];
            require(std::isfinite(value));
            require(value >= 0.0L);
            sum += value;
        }
        require(close(sum, 1.0L));
    }
    require(close(
        result.modularity,
        approximate::graph::community_modularity(vertex_count, edges, result.labels)
    ));
}

void boundary_tests(){
    using approximate::graph::markov_clustering;
    const long double infinity = std::numeric_limits<long double>::infinity();
    const long double nan = std::numeric_limits<long double>::quiet_NaN();
    const long double maximum = std::numeric_limits<long double>::max();
    require_throws<std::invalid_argument>([]{
        static_cast<void>(markov_clustering(2, {{0, 1, -1.0L}}));
    });
    require_throws<std::invalid_argument>([infinity]{
        static_cast<void>(markov_clustering(2, {{0, 1, infinity}}));
    });
    require_throws<std::invalid_argument>([nan]{
        static_cast<void>(markov_clustering(2, {{0, 1, nan}}));
    });
    require_throws<std::out_of_range>([]{
        static_cast<void>(markov_clustering(2, {{0, 2, 1.0L}}));
    });
    require_throws<std::overflow_error>([maximum]{
        static_cast<void>(markov_clustering(
            2, {{0, 1, maximum * 0.75L}, {1, 0, maximum * 0.75L}}
        ));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(markov_clustering(0, {}, 1));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(markov_clustering(0, {}, 2, 1.0L));
    });
    require_throws<std::invalid_argument>([infinity]{
        static_cast<void>(markov_clustering(0, {}, 2, infinity));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(markov_clustering(0, {}, 2, 2.0L, -1.0L));
    });
    require_throws<std::invalid_argument>([infinity]{
        static_cast<void>(markov_clustering(0, {}, 2, 2.0L, infinity));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(markov_clustering(0, {}, 2, 2.0L, 1.0L, -1.0L));
    });
    require_throws<std::invalid_argument>([infinity]{
        static_cast<void>(markov_clustering(0, {}, 2, 2.0L, 1.0L, infinity));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(markov_clustering(0, {}, 2, 2.0L, 1.0L, 0.0L, 1, -1.0L));
    });
    require_throws<std::invalid_argument>([infinity]{
        static_cast<void>(markov_clustering(0, {}, 2, 2.0L, 1.0L, 0.0L, 1, infinity));
    });

    const auto empty = markov_clustering(0, {});
    check_result(0, {}, empty, 100);
    const auto initial = markov_clustering(2, {{0, 1, 3.0L}}, 2, 2.0L, 1.0L, 0.0L, 0);
    check_result(2, {{0, 1, 3.0L}}, initial, 0);
    require(initial.iterations == 0 && !initial.converged);

    const std::vector<WeightedUndirectedEdge> noisy{
        {0, 0, 100.0L}, {0, 1, 1.0L}, {1, 0, 2.0L}, {0, 1, 0.0L}
    };
    const auto normalized = markov_clustering(2, noisy);
    const auto aggregated = markov_clustering(2, {{0, 1, 3.0L}});
    require(normalized.labels == aggregated.labels);
    require(normalized.transition_matrix == aggregated.transition_matrix);

    const auto large = markov_clustering(2, {{0, 1, maximum / 4.0L}}, 2, 2.0L, 1.0L, 0.0L, 2);
    check_result(2, {{0, 1, maximum / 4.0L}}, large, 2);
}

}  // namespace

int main(){
    using approximate::graph::markov_clustering;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t case_count = 0;
    std::cin >> case_count;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t identifier = 0;
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::size_t expansion_power = 0;
        long double inflation = 0.0L;
        long double self_loop_weight = 0.0L;
        long double pruning_threshold = 0.0L;
        std::size_t maximum_iterations = 0;
        long double tolerance = 0.0L;
        std::cin >> identifier >> vertex_count >> edge_count >> expansion_power >> inflation
                 >> self_loop_weight >> pruning_threshold >> maximum_iterations >> tolerance;
        std::vector<WeightedUndirectedEdge> edges(edge_count);
        for(auto& edge : edges) std::cin >> edge.left >> edge.right >> edge.weight;
        const auto result = markov_clustering(
            vertex_count, edges, expansion_power, inflation, self_loop_weight,
            pruning_threshold, maximum_iterations, tolerance
        );
        check_result(vertex_count, edges, result, maximum_iterations);
        const auto repeated = markov_clustering(
            vertex_count, edges, expansion_power, inflation, self_loop_weight,
            pruning_threshold, maximum_iterations, tolerance
        );
        require(result.labels == repeated.labels);
        require(result.transition_matrix == repeated.transition_matrix);
        std::cout << "CASE " << identifier << ' ' << result.community_count;
        for(const std::size_t label : result.labels) std::cout << ' ' << label;
        std::cout << '\n';
    }
}
