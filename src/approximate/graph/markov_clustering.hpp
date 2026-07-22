#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_MARKOV_CLUSTERING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_MARKOV_CLUSTERING_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "community_detection.hpp"

namespace approximate::graph {

struct MarkovClusteringResult {
    std::vector<std::size_t> labels;
    std::size_t community_count = 0;
    std::vector<std::vector<long double>> transition_matrix;
    long double modularity = 0.0L;
    std::size_t iterations = 0;
    bool converged = false;
};

namespace detail {

[[nodiscard]] inline std::vector<std::vector<long double>> multiply_stochastic_matrices(
    const std::vector<std::vector<long double>>& left,
    const std::vector<std::vector<long double>>& right
){
    const std::size_t size = left.size();
    std::vector<std::vector<long double>> result(size, std::vector<long double>(size, 0.0L));
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t middle = 0; middle < size; ++middle){
            if(left[row][middle] == 0.0L) continue;
            for(std::size_t column = 0; column < size; ++column){
                if(right[middle][column] == 0.0L) continue;
                const long double term = left[row][middle] * right[middle][column];
                result[row][column] = checked_nonnegative_weight_sum(result[row][column], term);
            }
        }
    }
    return result;
}

class MCLDisjointSet {
    std::vector<std::size_t> parent_;

public:
    explicit MCLDisjointSet(std::size_t size): parent_(size){
        std::iota(parent_.begin(), parent_.end(), std::size_t{0});
    }

    std::size_t leader(std::size_t vertex){
        while(parent_[vertex] != vertex){
            parent_[vertex] = parent_[parent_[vertex]];
            vertex = parent_[vertex];
        }
        return vertex;
    }

    void merge(std::size_t left, std::size_t right){
        left = leader(left);
        right = leader(right);
        if(left == right) return;
        if(left > right) std::swap(left, right);
        parent_[right] = left;
    }
};

}  // namespace detail

[[nodiscard]] inline MarkovClusteringResult markov_clustering(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::size_t expansion_power = 2,
    long double inflation = 2.0L,
    long double self_loop_weight = 1.0L,
    long double pruning_threshold = 1.0e-12L,
    std::size_t maximum_iterations = 100,
    long double tolerance = 1.0e-10L
){
    if(expansion_power < 2 || !std::isfinite(inflation) || inflation <= 1.0L
       || !std::isfinite(self_loop_weight) || self_loop_weight < 0.0L
       || !std::isfinite(pruning_threshold) || pruning_threshold < 0.0L
       || !std::isfinite(tolerance) || tolerance < 0.0L){
        throw std::invalid_argument("invalid Markov clustering parameter");
    }
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(
        vertex_count, edges
    );
    MarkovClusteringResult result;
    if(vertex_count == 0) return result;
    if(vertex_count > std::numeric_limits<std::size_t>::max() / vertex_count){
        throw std::length_error("too many vertices for a dense Markov matrix");
    }
    std::vector<std::vector<long double>> matrix(
        vertex_count, std::vector<long double>(vertex_count, 0.0L)
    );
    for(const WeightedUndirectedEdge& edge : graph.edges){
        matrix[edge.left][edge.right] = edge.weight;
        matrix[edge.right][edge.left] = edge.weight;
    }
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        matrix[vertex][vertex] = detail::checked_nonnegative_weight_sum(
            matrix[vertex][vertex], self_loop_weight
        );
    }
    const auto normalize_columns = [&](std::vector<std::vector<long double>>& values){
        for(std::size_t column = 0; column < vertex_count; ++column){
            long double sum = 0.0L;
            for(std::size_t row = 0; row < vertex_count; ++row){
                sum = detail::checked_nonnegative_weight_sum(sum, values[row][column]);
            }
            if(sum == 0.0L){
                values[column][column] = 1.0L;
                sum = 1.0L;
            }
            for(std::size_t row = 0; row < vertex_count; ++row) values[row][column] /= sum;
        }
    };
    normalize_columns(matrix);

    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        std::vector<std::vector<long double>> expanded = matrix;
        for(std::size_t power = 1; power < expansion_power; ++power){
            expanded = detail::multiply_stochastic_matrices(expanded, matrix);
        }
        for(std::size_t column = 0; column < vertex_count; ++column){
            std::size_t maximum_row = 0;
            long double maximum_value = -1.0L;
            for(std::size_t row = 0; row < vertex_count; ++row){
                const long double value = std::pow(expanded[row][column], inflation);
                if(!std::isfinite(value)){
                    throw std::overflow_error("Markov inflation overflowed");
                }
                expanded[row][column] = value;
                if(value > maximum_value){
                    maximum_value = value;
                    maximum_row = row;
                }
            }
            for(std::size_t row = 0; row < vertex_count; ++row){
                if(row != maximum_row && expanded[row][column] < pruning_threshold){
                    expanded[row][column] = 0.0L;
                }
            }
        }
        normalize_columns(expanded);
        long double maximum_change = 0.0L;
        for(std::size_t row = 0; row < vertex_count; ++row){
            for(std::size_t column = 0; column < vertex_count; ++column){
                maximum_change = std::max(
                    maximum_change, std::abs(expanded[row][column] - matrix[row][column])
                );
            }
        }
        matrix = std::move(expanded);
        result.iterations = iteration + 1;
        if(maximum_change <= tolerance){
            result.converged = true;
            break;
        }
    }

    detail::MCLDisjointSet dsu(vertex_count);
    for(std::size_t column = 0; column < vertex_count; ++column){
        std::size_t attractor = 0;
        for(std::size_t row = 1; row < vertex_count; ++row){
            if(matrix[row][column] > matrix[attractor][column]) attractor = row;
        }
        dsu.merge(column, attractor);
    }
    std::vector<std::size_t> root_label(vertex_count, vertex_count);
    result.labels.resize(vertex_count);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        const std::size_t root = dsu.leader(vertex);
        if(root_label[root] == vertex_count) root_label[root] = result.community_count++;
        result.labels[vertex] = root_label[root];
    }
    result.modularity = community_modularity(vertex_count, edges, result.labels);
    result.transition_matrix = std::move(matrix);
    return result;
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_MARKOV_CLUSTERING_HPP_INCLUDED
