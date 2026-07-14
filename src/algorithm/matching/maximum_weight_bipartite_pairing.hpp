#pragma once

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

struct MaximumWeightBipartitePairingEdge{
    int left;
    int right;
    long long weight;
};

inline long long maximum_weight_bipartite_pairing(
    int left_size,
    int right_size,
    const std::vector<MaximumWeightBipartitePairingEdge>& input_edges
){
    if(left_size < 0 || right_size < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (maximum_weight_bipartite_pairing)."
        );
    }

    const int size = std::max(left_size, right_size);
    if(size == 0) return 0;
    std::vector<std::vector<long long>> weight(
        static_cast<std::size_t>(size),
        std::vector<long long>(static_cast<std::size_t>(size), 0)
    );
    long long maximum_weight = 0;
    for(const auto& edge: input_edges){
        if(edge.left < 0 || left_size <= edge.left ||
           edge.right < 0 || right_size <= edge.right || edge.weight < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (maximum_weight_bipartite_pairing)."
            );
        }
        auto& value = weight[static_cast<std::size_t>(edge.left)]
                            [static_cast<std::size_t>(edge.right)];
        value = std::max(value, edge.weight);
        maximum_weight = std::max(maximum_weight, edge.weight);
    }

    using Wide = __int128_t;
    const Wide infinity = Wide(1) << 120;
    std::vector<Wide> left_potential(static_cast<std::size_t>(size + 1), 0);
    std::vector<Wide> right_potential(static_cast<std::size_t>(size + 1), 0);
    std::vector<Wide> minimum(static_cast<std::size_t>(size + 1), infinity);
    std::vector<int> matched_row(static_cast<std::size_t>(size + 1), 0);
    std::vector<int> previous_column(static_cast<std::size_t>(size + 1), 0);
    std::vector<char> used(static_cast<std::size_t>(size + 1), 0);

    for(int row = 1; row <= size; row++){
        matched_row[0] = row;
        std::fill(minimum.begin(), minimum.end(), infinity);
        std::fill(used.begin(), used.end(), 0);
        int column = 0;
        do{
            used[static_cast<std::size_t>(column)] = 1;
            const int current_row = matched_row[static_cast<std::size_t>(column)];
            Wide delta = infinity;
            int next_column = 0;
            for(int candidate = 1; candidate <= size; candidate++){
                if(used[static_cast<std::size_t>(candidate)]) continue;
                const Wide cost = Wide(maximum_weight) -
                    Wide(weight[static_cast<std::size_t>(current_row - 1)]
                               [static_cast<std::size_t>(candidate - 1)]);
                const Wide reduced = cost -
                    left_potential[static_cast<std::size_t>(current_row)] -
                    right_potential[static_cast<std::size_t>(candidate)];
                if(reduced < minimum[static_cast<std::size_t>(candidate)]){
                    minimum[static_cast<std::size_t>(candidate)] = reduced;
                    previous_column[static_cast<std::size_t>(candidate)] = column;
                }
                if(minimum[static_cast<std::size_t>(candidate)] < delta){
                    delta = minimum[static_cast<std::size_t>(candidate)];
                    next_column = candidate;
                }
            }
            for(int candidate = 0; candidate <= size; candidate++){
                if(used[static_cast<std::size_t>(candidate)]){
                    left_potential[static_cast<std::size_t>(
                        matched_row[static_cast<std::size_t>(candidate)]
                    )] += delta;
                    right_potential[static_cast<std::size_t>(candidate)] -= delta;
                }
                else{
                    minimum[static_cast<std::size_t>(candidate)] -= delta;
                }
            }
            column = next_column;
        }while(matched_row[static_cast<std::size_t>(column)] != 0);

        do{
            const int next_column = previous_column[static_cast<std::size_t>(column)];
            matched_row[static_cast<std::size_t>(column)] =
                matched_row[static_cast<std::size_t>(next_column)];
            column = next_column;
        }while(column != 0);
    }

    Wide answer = 0;
    for(int column = 1; column <= size; column++){
        const int row = matched_row[static_cast<std::size_t>(column)];
        answer += weight[static_cast<std::size_t>(row - 1)]
                        [static_cast<std::size_t>(column - 1)];
    }
    if(answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "library assertion fault: overflow (maximum_weight_bipartite_pairing)."
        );
    }
    return static_cast<long long>(answer);
}
