#ifndef CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_TREE_DISTANCE_SUMS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_TREE_DISTANCE_SUMS_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "rerooting_dp.hpp"

namespace tree_distance_sums_internal{

struct DistanceSumMonoid{
    using S = std::pair<long long, long long>;

    constexpr S op(const S& left, const S& right) const{
        return {left.first + right.first, left.second + right.second};
    }

    constexpr S e() const{
        return {0, 0};
    }
};

inline constexpr DistanceSumMonoid distance_sum_monoid{};

struct AddVertex{
    std::pair<long long, long long> operator()(
        std::pair<long long, long long> value,
        int
    ) const{
        value.first++;
        return value;
    }
};

struct AddEdge{
    std::pair<long long, long long> operator()(
        std::pair<long long, long long> value,
        int,
        int,
        int
    ) const{
        value.second += value.first;
        return value;
    }
};

} // namespace tree_distance_sums_internal

inline std::vector<long long> tree_distance_sums(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative vertex count "
            "(tree_distance_sums)."
        );
    }
    using namespace tree_distance_sums_internal;
    RerootingDP<distance_sum_monoid, AddVertex, AddEdge> rerooting(
        n, AddVertex{}, AddEdge{}
    );
    for(const auto& [left, right]: edges){
        rerooting.add_edge(left, right);
    }
    const auto values = rerooting.solve();
    std::vector<long long> answer;
    answer.reserve(values.size());
    for(const auto& [vertex_count, distance_sum]: values){
        static_cast<void>(vertex_count);
        answer.push_back(distance_sum);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_DYNAMIC_PROGRAMMING_TREE_DISTANCE_SUMS_HPP_INCLUDED
