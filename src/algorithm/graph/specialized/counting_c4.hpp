#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_COUNTING_C4_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_COUNTING_C4_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace counting_c4_internal{

inline std::vector<long long> simple_graph(
    int vertex_count,
    std::vector<int> from,
    std::vector<int> to,
    const std::vector<long long>& multiplicity
){
    const int edge_count = static_cast<int>(multiplicity.size());
    std::vector<int> degree(static_cast<std::size_t>(vertex_count));
    for(int edge = 0; edge < edge_count; ++edge){
        ++degree[static_cast<std::size_t>(from[static_cast<std::size_t>(edge)])];
        ++degree[static_cast<std::size_t>(to[static_cast<std::size_t>(edge)])];
    }
    std::vector<int> order(static_cast<std::size_t>(vertex_count));
    std::iota(order.begin(), order.end(), 0);
    std::stable_sort(order.begin(), order.end(), [&](int left, int right){
        return degree[static_cast<std::size_t>(left)] <
            degree[static_cast<std::size_t>(right)];
    });
    std::vector<int> rank(static_cast<std::size_t>(vertex_count));
    for(int index = 0; index < vertex_count; ++index){
        rank[static_cast<std::size_t>(
            order[static_cast<std::size_t>(index)]
        )] = index;
    }
    for(int edge = 0; edge < edge_count; ++edge){
        from[static_cast<std::size_t>(edge)] = rank[
            static_cast<std::size_t>(from[static_cast<std::size_t>(edge)])
        ];
        to[static_cast<std::size_t>(edge)] = rank[
            static_cast<std::size_t>(to[static_cast<std::size_t>(edge)])
        ];
        if(from[static_cast<std::size_t>(edge)] <
           to[static_cast<std::size_t>(edge)]){
            std::swap(
                from[static_cast<std::size_t>(edge)],
                to[static_cast<std::size_t>(edge)]
            );
        }
    }

    std::vector<int> start(static_cast<std::size_t>(vertex_count));
    for(int vertex = 0; vertex + 1 < vertex_count; ++vertex){
        start[static_cast<std::size_t>(vertex + 1)] =
            start[static_cast<std::size_t>(vertex)] +
            degree[static_cast<std::size_t>(
                order[static_cast<std::size_t>(vertex)]
            )];
    }
    std::vector<int> end = start;
    std::vector<int> edge_id(static_cast<std::size_t>(2 * edge_count));
    std::vector<int> endpoint(static_cast<std::size_t>(2 * edge_count));
    for(int edge = 0; edge < edge_count; ++edge){
        const int vertex = from[static_cast<std::size_t>(edge)];
        const int position = end[static_cast<std::size_t>(vertex)]++;
        edge_id[static_cast<std::size_t>(position)] = edge;
        endpoint[static_cast<std::size_t>(position)] =
            to[static_cast<std::size_t>(edge)];
    }
    const std::vector<int> initial_end = end;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        for(int position = start[static_cast<std::size_t>(vertex)];
            position < initial_end[static_cast<std::size_t>(vertex)];
            ++position){
            const int edge = edge_id[static_cast<std::size_t>(position)];
            const int neighbor = endpoint[static_cast<std::size_t>(position)];
            const int reverse_position = end[static_cast<std::size_t>(neighbor)]++;
            edge_id[static_cast<std::size_t>(reverse_position)] = edge;
            endpoint[static_cast<std::size_t>(reverse_position)] = vertex;
        }
    }

    std::vector<long long> path_count(
        static_cast<std::size_t>(vertex_count)
    );
    std::vector<long long> answer(static_cast<std::size_t>(edge_count));
    for(int vertex = vertex_count - 1; vertex >= 0; --vertex){
        for(int position = start[static_cast<std::size_t>(vertex)];
            position < end[static_cast<std::size_t>(vertex)]; ++position){
            const int first_edge = edge_id[static_cast<std::size_t>(position)];
            const int middle = endpoint[static_cast<std::size_t>(position)];
            --end[static_cast<std::size_t>(middle)];
            for(int next = start[static_cast<std::size_t>(middle)];
                next < end[static_cast<std::size_t>(middle)]; ++next){
                const int second_edge = edge_id[static_cast<std::size_t>(next)];
                const int last = endpoint[static_cast<std::size_t>(next)];
                path_count[static_cast<std::size_t>(last)] +=
                    multiplicity[static_cast<std::size_t>(first_edge)] *
                    multiplicity[static_cast<std::size_t>(second_edge)];
            }
        }
        for(int position = start[static_cast<std::size_t>(vertex)];
            position < end[static_cast<std::size_t>(vertex)]; ++position){
            const int first_edge = edge_id[static_cast<std::size_t>(position)];
            const int middle = endpoint[static_cast<std::size_t>(position)];
            for(int next = start[static_cast<std::size_t>(middle)];
                next < end[static_cast<std::size_t>(middle)]; ++next){
                const int second_edge = edge_id[static_cast<std::size_t>(next)];
                const int last = endpoint[static_cast<std::size_t>(next)];
                const long long value =
                    path_count[static_cast<std::size_t>(last)] -
                    multiplicity[static_cast<std::size_t>(first_edge)] *
                    multiplicity[static_cast<std::size_t>(second_edge)];
                answer[static_cast<std::size_t>(first_edge)] += value *
                    multiplicity[static_cast<std::size_t>(second_edge)];
                answer[static_cast<std::size_t>(second_edge)] += value *
                    multiplicity[static_cast<std::size_t>(first_edge)];
            }
        }
        for(int position = start[static_cast<std::size_t>(vertex)];
            position < end[static_cast<std::size_t>(vertex)]; ++position){
            const int middle = endpoint[static_cast<std::size_t>(position)];
            for(int next = start[static_cast<std::size_t>(middle)];
                next < end[static_cast<std::size_t>(middle)]; ++next){
                path_count[static_cast<std::size_t>(
                    endpoint[static_cast<std::size_t>(next)]
                )] = 0;
            }
        }
    }
    return answer;
}

} // namespace counting_c4_internal

inline std::vector<long long> count_c4_per_edge(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    constexpr const char* message =
        "library assertion fault: range violation (count_c4_per_edge).";
    if(vertex_count < 0)[[unlikely]] throw std::runtime_error(message);
    const int edge_count = static_cast<int>(edges.size());
    std::vector<int> order(static_cast<std::size_t>(edge_count));
    std::iota(order.begin(), order.end(), 0);
    std::vector<std::pair<int, int>> normalized = edges;
    for(auto& [left, right]: normalized){
        if(left < 0 || vertex_count <= left ||
           right < 0 || vertex_count <= right || left == right)[[unlikely]]{
            throw std::runtime_error(message);
        }
        if(left > right) std::swap(left, right);
    }
    std::sort(order.begin(), order.end(), [&](int left, int right){
        return normalized[static_cast<std::size_t>(left)] <
            normalized[static_cast<std::size_t>(right)];
    });

    std::vector<int> compressed_from;
    std::vector<int> compressed_to;
    std::vector<long long> multiplicity;
    std::vector<int> compressed_id(static_cast<std::size_t>(edge_count));
    for(int edge: order){
        const auto [left, right] = normalized[static_cast<std::size_t>(edge)];
        if(compressed_from.empty() || compressed_from.back() != left ||
           compressed_to.back() != right){
            compressed_from.push_back(left);
            compressed_to.push_back(right);
            multiplicity.push_back(0);
        }
        ++multiplicity.back();
        compressed_id[static_cast<std::size_t>(edge)] =
            static_cast<int>(multiplicity.size()) - 1;
    }

    const auto compressed_answer = counting_c4_internal::simple_graph(
        vertex_count, std::move(compressed_from), std::move(compressed_to),
        multiplicity
    );
    std::vector<long long> answer(static_cast<std::size_t>(edge_count));
    for(int edge = 0; edge < edge_count; ++edge){
        answer[static_cast<std::size_t>(edge)] = compressed_answer[
            static_cast<std::size_t>(compressed_id[static_cast<std::size_t>(edge)])
        ];
    }
    return answer;
}

inline __int128_t count_c4(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    const auto per_edge = count_c4_per_edge(vertex_count, edges);
    __int128_t total = 0;
    for(long long value: per_edge) total += value;
    return total / 4;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_COUNTING_C4_HPP_INCLUDED
