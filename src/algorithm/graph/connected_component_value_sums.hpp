#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTED_COMPONENT_VALUE_SUMS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTED_COMPONENT_VALUE_SUMS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

inline std::vector<long long> connected_component_value_sums(
    const std::vector<long long>& values,
    const std::vector<std::pair<int, int>>& edges
){
    if(values.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("connected component graph is too large");
    }
    const int size = static_cast<int>(values.size());
    std::vector<int> parent(static_cast<std::size_t>(size), -1);
    const auto leader = [&](auto&& self, int vertex) -> int {
        if(parent[static_cast<std::size_t>(vertex)] < 0) return vertex;
        return parent[static_cast<std::size_t>(vertex)] =
            self(self, parent[static_cast<std::size_t>(vertex)]);
    };
    for(const auto [left_vertex, right_vertex]: edges){
        if(left_vertex < 0 || size <= left_vertex || right_vertex < 0 ||
           size <= right_vertex)[[unlikely]]{
            throw std::out_of_range(
                "connected_component_value_sums edge out of range"
            );
        }
        int left = leader(leader, left_vertex);
        int right = leader(leader, right_vertex);
        if(left == right) continue;
        if(parent[static_cast<std::size_t>(right)] <
           parent[static_cast<std::size_t>(left)]){
            std::swap(left, right);
        }
        parent[static_cast<std::size_t>(left)] +=
            parent[static_cast<std::size_t>(right)];
        parent[static_cast<std::size_t>(right)] = left;
    }
    std::vector<long long> sums(static_cast<std::size_t>(size));
    for(int vertex = 0; vertex < size; ++vertex){
        const int root = leader(leader, vertex);
        const long long value = values[static_cast<std::size_t>(vertex)];
        long long& sum = sums[static_cast<std::size_t>(root)];
        if((value > 0 && sum > std::numeric_limits<long long>::max() - value) ||
           (value < 0 && sum < std::numeric_limits<long long>::min() - value)){
            throw std::overflow_error("connected component sum overflow");
        }
        sum += value;
    }
    std::vector<long long> answer;
    for(int vertex = 0; vertex < size; ++vertex){
        if(parent[static_cast<std::size_t>(vertex)] < 0){
            answer.push_back(sums[static_cast<std::size_t>(vertex)]);
        }
    }
    std::sort(answer.begin(), answer.end());
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTED_COMPONENT_VALUE_SUMS_HPP_INCLUDED
