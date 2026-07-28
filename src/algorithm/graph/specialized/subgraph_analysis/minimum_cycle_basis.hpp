#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_SUBGRAPH_ANALYSIS_MINIMUM_CYCLE_BASIS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_SUBGRAPH_ANALYSIS_MINIMUM_CYCLE_BASIS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

struct MinimumCycleBasisEdge{
    int from;
    int to;
    long long weight;
};

struct MinimumCycleBasisResult{
    long long weight;
    std::vector<std::vector<int>> cycles;
};

namespace minimum_cycle_basis_internal{

struct Candidate{
    __int128 weight;
    std::vector<std::uint64_t> bits;
};

inline bool bit_vector_less(
    const std::vector<std::uint64_t>& left,
    const std::vector<std::uint64_t>& right
){
    for(std::size_t index = left.size(); index-- > 0; ){
        if(left[index] != right[index]) return left[index] < right[index];
    }
    return false;
}

} // namespace minimum_cycle_basis_internal

inline MinimumCycleBasisResult minimum_cycle_basis(
    int vertex_count,
    const std::vector<MinimumCycleBasisEdge>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (minimum_cycle_basis)."
        );
    }
    const int edge_count = static_cast<int>(edges.size());
    std::vector<std::vector<std::pair<int, int>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(int edge_id = 0; edge_id < edge_count; ++edge_id){
        const auto& edge = edges[static_cast<std::size_t>(edge_id)];
        if(edge.from < 0 || vertex_count <= edge.from
            || edge.to < 0 || vertex_count <= edge.to
            || edge.weight < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(minimum_cycle_basis)."
            );
        }
        graph[static_cast<std::size_t>(edge.from)].emplace_back(
            edge.to, edge_id
        );
        if(edge.from != edge.to){
            graph[static_cast<std::size_t>(edge.to)].emplace_back(
                edge.from, edge_id
            );
        }
    }
    const std::size_t word_count =
        (static_cast<std::size_t>(edge_count) + 63U) / 64U;
    using Wide = __int128;
    constexpr Wide infinity = Wide{1} << 120;
    using QueueEntry = std::tuple<Wide, int, int>;
    std::vector<minimum_cycle_basis_internal::Candidate> candidates;
    candidates.reserve(
        static_cast<std::size_t>(vertex_count)
            * static_cast<std::size_t>(edge_count)
    );
    for(int source = 0; source < vertex_count; ++source){
        std::vector<Wide> distance(
            static_cast<std::size_t>(vertex_count), infinity
        );
        std::vector<int> hop_count(
            static_cast<std::size_t>(vertex_count),
            std::numeric_limits<int>::max()
        );
        std::vector<int> parent(
            static_cast<std::size_t>(vertex_count), -1
        );
        std::vector<int> parent_edge(
            static_cast<std::size_t>(vertex_count), -1
        );
        std::priority_queue<
            QueueEntry,
            std::vector<QueueEntry>,
            std::greater<QueueEntry>
        > queue;
        distance[static_cast<std::size_t>(source)] = 0;
        hop_count[static_cast<std::size_t>(source)] = 0;
        queue.emplace(0, 0, source);
        while(!queue.empty()){
            auto [current_distance, current_hops, vertex] = queue.top();
            queue.pop();
            if(current_distance != distance[static_cast<std::size_t>(vertex)]
                || current_hops
                    != hop_count[static_cast<std::size_t>(vertex)]){
                continue;
            }
            for(auto [to, edge_id]: graph[static_cast<std::size_t>(vertex)]){
                const auto& edge = edges[static_cast<std::size_t>(edge_id)];
                Wide next_distance =
                    current_distance + static_cast<Wide>(edge.weight);
                int next_hops = current_hops + 1;
                bool improve =
                    next_distance < distance[static_cast<std::size_t>(to)]
                    || (next_distance
                            == distance[static_cast<std::size_t>(to)]
                        && (next_hops
                                < hop_count[static_cast<std::size_t>(to)]
                            || (next_hops
                                    == hop_count[static_cast<std::size_t>(to)]
                                && edge_id
                                    < parent_edge[
                                        static_cast<std::size_t>(to)
                                    ])));
                if(!improve) continue;
                distance[static_cast<std::size_t>(to)] = next_distance;
                hop_count[static_cast<std::size_t>(to)] = next_hops;
                parent[static_cast<std::size_t>(to)] = vertex;
                parent_edge[static_cast<std::size_t>(to)] = edge_id;
                queue.emplace(next_distance, next_hops, to);
            }
        }
        std::vector<int> order(static_cast<std::size_t>(vertex_count));
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int left, int right){
            return hop_count[static_cast<std::size_t>(left)]
                < hop_count[static_cast<std::size_t>(right)];
        });
        std::vector<std::vector<std::uint64_t>> path(
            static_cast<std::size_t>(vertex_count),
            std::vector<std::uint64_t>(word_count, 0)
        );
        for(int vertex: order){
            int edge_id = parent_edge[static_cast<std::size_t>(vertex)];
            if(edge_id == -1) continue;
            int from = parent[static_cast<std::size_t>(vertex)];
            path[static_cast<std::size_t>(vertex)] =
                path[static_cast<std::size_t>(from)];
            path[static_cast<std::size_t>(vertex)]
                [static_cast<std::size_t>(edge_id) >> 6U] ^=
                    std::uint64_t{1}
                        << (static_cast<unsigned>(edge_id) & 63U);
        }
        for(int edge_id = 0; edge_id < edge_count; ++edge_id){
            const auto& edge = edges[static_cast<std::size_t>(edge_id)];
            if(distance[static_cast<std::size_t>(edge.from)] == infinity
                || distance[static_cast<std::size_t>(edge.to)] == infinity){
                continue;
            }
            std::vector<std::uint64_t> bits(word_count);
            bool nonzero = false;
            for(std::size_t word = 0; word < word_count; ++word){
                bits[word] =
                    path[static_cast<std::size_t>(edge.from)][word]
                    ^ path[static_cast<std::size_t>(edge.to)][word];
            }
            bits[static_cast<std::size_t>(edge_id) >> 6U] ^=
                std::uint64_t{1}
                    << (static_cast<unsigned>(edge_id) & 63U);
            Wide weight = 0;
            for(int id = 0; id < edge_count; ++id){
                if(((bits[static_cast<std::size_t>(id) >> 6U]
                    >> (static_cast<unsigned>(id) & 63U)) & 1U) != 0){
                    nonzero = true;
                    weight += static_cast<Wide>(
                        edges[static_cast<std::size_t>(id)].weight
                    );
                }
            }
            if(nonzero){
                candidates.push_back({weight, std::move(bits)});
            }
        }
    }
    std::sort(candidates.begin(), candidates.end(), [](const auto& left,
                                                        const auto& right){
        if(left.weight != right.weight) return left.weight < right.weight;
        return minimum_cycle_basis_internal::bit_vector_less(
            left.bits, right.bits
        );
    });
    candidates.erase(
        std::unique(candidates.begin(), candidates.end(), [](const auto& left,
                                                             const auto& right){
            return left.bits == right.bits;
        }),
        candidates.end()
    );

    std::vector<std::vector<std::uint64_t>> basis(
        static_cast<std::size_t>(edge_count)
    );
    std::vector<int> component_parent(
        static_cast<std::size_t>(vertex_count)
    );
    std::iota(component_parent.begin(), component_parent.end(), 0);
    auto component_leader = [&](int vertex){
        while(component_parent[static_cast<std::size_t>(vertex)] != vertex){
            component_parent[static_cast<std::size_t>(vertex)] =
                component_parent[static_cast<std::size_t>(
                    component_parent[static_cast<std::size_t>(vertex)]
                )];
            vertex = component_parent[static_cast<std::size_t>(vertex)];
        }
        return vertex;
    };
    int component_count = vertex_count;
    for(const auto& edge: edges){
        int left = component_leader(edge.from);
        int right = component_leader(edge.to);
        if(left != right){
            component_parent[static_cast<std::size_t>(right)] = left;
            --component_count;
        }
    }
    const int target_rank = edge_count - vertex_count + component_count;
    MinimumCycleBasisResult result{0, {}};
    Wide total_weight = 0;
    for(const auto& candidate: candidates){
        std::vector<std::uint64_t> reduced = candidate.bits;
        int pivot = -1;
        for(int edge_id = edge_count; edge_id-- > 0; ){
            if(((reduced[static_cast<std::size_t>(edge_id) >> 6U]
                >> (static_cast<unsigned>(edge_id) & 63U)) & 1U) == 0){
                continue;
            }
            if(basis[static_cast<std::size_t>(edge_id)].empty()){
                pivot = edge_id;
                break;
            }
            for(std::size_t word = 0; word < word_count; ++word){
                reduced[word] ^=
                    basis[static_cast<std::size_t>(edge_id)][word];
            }
        }
        if(pivot == -1) continue;
        basis[static_cast<std::size_t>(pivot)] = std::move(reduced);
        std::vector<int> cycle;
        for(int edge_id = 0; edge_id < edge_count; ++edge_id){
            if(((candidate.bits[static_cast<std::size_t>(edge_id) >> 6U]
                >> (static_cast<unsigned>(edge_id) & 63U)) & 1U) != 0){
                cycle.push_back(edge_id);
            }
        }
        result.cycles.push_back(std::move(cycle));
        total_weight += candidate.weight;
        if(static_cast<int>(result.cycles.size()) == target_rank) break;
    }
    if(total_weight > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "minimum_cycle_basis total weight overflows long long"
        );
    }
    result.weight = static_cast<long long>(total_weight);
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_SUBGRAPH_ANALYSIS_MINIMUM_CYCLE_BASIS_HPP_INCLUDED
