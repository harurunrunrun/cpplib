#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_FIDUCCIA_MATTHEYSES_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_FIDUCCIA_MATTHEYSES_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

#include "graph_partition.hpp"

namespace approximate::graph {
namespace detail {

struct FmQueueEntry {
    long double gain;
    std::size_t vertex;
    std::size_t version;
};

struct FmQueueCompare {
    [[nodiscard]] bool operator()(const FmQueueEntry& left, const FmQueueEntry& right) const {
        if(left.gain != right.gain) return left.gain < right.gain;
        return left.vertex > right.vertex;
    }
};

[[nodiscard]] inline bool fm_move_is_feasible(
    const std::array<std::size_t, 2>& count,
    unsigned char source,
    std::size_t maximum_imbalance
){
    if(count[source] == 0) return false;
    std::array<std::size_t, 2> next = count;
    --next[source];
    ++next[source ^ 1U];
    const std::size_t difference = next[0] > next[1] ? next[0] - next[1] : next[1] - next[0];
    return difference <= maximum_imbalance;
}

}  // namespace detail

[[nodiscard]] inline BipartitionResult fiduccia_mattheyses(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::vector<unsigned char> initial_side,
    std::size_t maximum_imbalance = 2,
    std::size_t maximum_passes = 20
){
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    detail::validate_partition(initial_side, vertex_count);
    detail::validate_maximum_imbalance(vertex_count, maximum_imbalance);
    if(detail::partition_imbalance(initial_side) > maximum_imbalance){
        throw std::invalid_argument("initial partition exceeds the maximum imbalance");
    }
    std::vector<unsigned char> side = std::move(initial_side);
    if(vertex_count < 2 || graph.total_edge_weight == 0.0L || maximum_passes == 0){
        return detail::make_bipartition_result(graph, std::move(side));
    }

    using Queue = std::priority_queue<detail::FmQueueEntry, std::vector<detail::FmQueueEntry>, detail::FmQueueCompare>;
    for(std::size_t pass = 0; pass < maximum_passes; ++pass){
        const std::vector<unsigned char> pass_start = side;
        std::array<std::size_t, 2> count{0, 0};
        for(const unsigned char value : side) ++count[value];
        std::vector<long double> gain(vertex_count, 0.0L);
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            for(const detail::WeightedNeighbor& neighbor : graph.adjacency[vertex]){
                gain[vertex] += side[vertex] == side[neighbor.vertex]
                    ? -neighbor.normalized_weight
                    : neighbor.normalized_weight;
            }
        }
        std::vector<std::size_t> version(vertex_count, 0);
        std::vector<bool> locked(vertex_count, false);
        std::array<Queue, 2> queues;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            queues[side[vertex]].push({gain[vertex], vertex, 0});
        }
        std::vector<std::size_t> moves;
        std::vector<long double> move_gains;
        moves.reserve(vertex_count);
        move_gains.reserve(vertex_count);

        const auto clean_top = [&](unsigned char source, Queue& queue){
            while(!queue.empty()){
                const detail::FmQueueEntry& entry = queue.top();
                if(locked[entry.vertex] || side[entry.vertex] != source || version[entry.vertex] != entry.version){
                    queue.pop();
                }else{
                    break;
                }
            }
        };

        for(std::size_t step = 0; step < vertex_count; ++step){
            clean_top(0, queues[0]);
            clean_top(1, queues[1]);
            bool found = false;
            detail::FmQueueEntry chosen{};
            unsigned char source = 0;
            for(unsigned char candidate_source = 0; candidate_source < 2; ++candidate_source){
                if(!detail::fm_move_is_feasible(count, candidate_source, maximum_imbalance)
                   || queues[candidate_source].empty()){
                    continue;
                }
                const detail::FmQueueEntry candidate = queues[candidate_source].top();
                if(!found || candidate.gain > chosen.gain
                   || (candidate.gain == chosen.gain && candidate.vertex < chosen.vertex)){
                    found = true;
                    chosen = candidate;
                    source = candidate_source;
                }
            }
            if(!found) break;
            queues[source].pop();
            const std::size_t vertex = chosen.vertex;
            locked[vertex] = true;
            moves.push_back(vertex);
            move_gains.push_back(gain[vertex]);
            --count[source];
            ++count[source ^ 1U];
            side[vertex] = static_cast<unsigned char>(source ^ 1U);

            for(const detail::WeightedNeighbor& neighbor : graph.adjacency[vertex]){
                if(locked[neighbor.vertex]) continue;
                gain[neighbor.vertex] += side[neighbor.vertex] == source
                    ? 2.0L * neighbor.normalized_weight
                    : -2.0L * neighbor.normalized_weight;
                ++version[neighbor.vertex];
                queues[side[neighbor.vertex]].push({gain[neighbor.vertex], neighbor.vertex, version[neighbor.vertex]});
            }
        }

        long double cumulative_gain = 0.0L;
        long double best_cumulative_gain = 0.0L;
        std::size_t best_prefix = 0;
        for(std::size_t index = 0; index < move_gains.size(); ++index){
            cumulative_gain += move_gains[index];
            if(cumulative_gain > best_cumulative_gain){
                best_cumulative_gain = cumulative_gain;
                best_prefix = index + 1;
            }
        }
        side = pass_start;
        if(best_prefix == 0) break;
        for(std::size_t index = 0; index < best_prefix; ++index) side[moves[index]] ^= 1U;
        if(detail::normalized_cut_weight(graph, side) >= detail::normalized_cut_weight(graph, pass_start)){
            side = pass_start;
            break;
        }
    }
    return detail::make_bipartition_result(graph, std::move(side));
}

[[nodiscard]] inline BipartitionResult fiduccia_mattheyses(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::size_t maximum_imbalance = 2,
    std::size_t maximum_passes = 20
){
    detail::validate_maximum_imbalance(vertex_count, maximum_imbalance);
    return fiduccia_mattheyses(
        vertex_count, edges, balanced_bipartition(vertex_count), maximum_imbalance, maximum_passes
    );
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_FIDUCCIA_MATTHEYSES_HPP_INCLUDED
