#pragma once

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "kruskal_mst.hpp"

template<class T>
using PairwiseDisconnectCostEdge = KruskalEdge<T>;

namespace pairwise_disconnect_cost_sum_internal{

struct Dsu{
    std::vector<int> parent_or_size;

    explicit Dsu(int n): parent_or_size(static_cast<std::size_t>(n), -1){}

    int leader(int vertex){
        if(parent_or_size[static_cast<std::size_t>(vertex)] < 0) return vertex;
        return parent_or_size[static_cast<std::size_t>(vertex)] = leader(
            parent_or_size[static_cast<std::size_t>(vertex)]
        );
    }

    std::pair<long long, long long> merge_sizes(int left, int right){
        left = leader(left);
        right = leader(right);
        if(left == right) return {0, 0};
        if(parent_or_size[static_cast<std::size_t>(left)] >
           parent_or_size[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        const long long left_size =
            -parent_or_size[static_cast<std::size_t>(left)];
        const long long right_size =
            -parent_or_size[static_cast<std::size_t>(right)];
        parent_or_size[static_cast<std::size_t>(left)] +=
            parent_or_size[static_cast<std::size_t>(right)];
        parent_or_size[static_cast<std::size_t>(right)] = left;
        return {left_size, right_size};
    }
};

inline long long add_mod(long long left, long long right, long long modulus){
    return static_cast<long long>(
        (static_cast<__int128>(left) + right) % modulus
    );
}

inline long long multiply_mod(long long left, long long right, long long modulus){
    return static_cast<long long>(
        static_cast<__int128>(left) * right % modulus
    );
}

} // namespace pairwise_disconnect_cost_sum_internal

template<class T>
long long pairwise_disconnect_cost_sum(
    int n,
    const std::vector<PairwiseDisconnectCostEdge<T>>& edges,
    long long modulus
){
    static_assert(std::is_integral_v<T>);
    if(n < 0 || modulus <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(pairwise_disconnect_cost_sum)."
        );
    }
    for(const auto& edge: edges){
        if(edge.from < 0 || n <= edge.from ||
           edge.to < 0 || n <= edge.to ||
           (std::is_signed_v<T> && edge.cost < T(0)))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(pairwise_disconnect_cost_sum)."
            );
        }
    }

    std::vector<int> order(edges.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int left, int right){
        return edges[static_cast<std::size_t>(left)].cost <
            edges[static_cast<std::size_t>(right)].cost;
    });
    for(std::size_t index = 1; index < order.size(); index++){
        if(edges[static_cast<std::size_t>(order[index - 1])].cost ==
           edges[static_cast<std::size_t>(order[index])].cost)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: duplicate edge weights "
                "(pairwise_disconnect_cost_sum)."
            );
        }
    }

    std::vector<long long> prefix_sum_mod(edges.size());
    long long prefix = 0;
    for(std::size_t index = 0; index < order.size(); index++){
        const auto& edge = edges[static_cast<std::size_t>(order[index])];
        const long long weight_mod = static_cast<long long>(
            static_cast<__int128>(edge.cost) % modulus
        );
        prefix = pairwise_disconnect_cost_sum_internal::add_mod(
            prefix, weight_mod, modulus
        );
        prefix_sum_mod[index] = prefix;
    }

    pairwise_disconnect_cost_sum_internal::Dsu dsu(n);
    long long answer = 0;
    for(std::size_t index = order.size(); index-- > 0;){
        const auto& edge = edges[static_cast<std::size_t>(order[index])];
        auto [left_size, right_size] = dsu.merge_sizes(
            edge.from, edge.to
        );
        if(left_size == 0) continue;
        const long long pair_count_mod =
            pairwise_disconnect_cost_sum_internal::multiply_mod(
                left_size % modulus, right_size % modulus, modulus
            );
        const long long contribution =
            pairwise_disconnect_cost_sum_internal::multiply_mod(
                pair_count_mod, prefix_sum_mod[index], modulus
            );
        answer = pairwise_disconnect_cost_sum_internal::add_mod(
            answer, contribution, modulus
        );
    }
    return answer;
}
