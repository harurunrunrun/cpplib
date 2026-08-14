#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_OFFLINE_RANGE_MEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_OFFLINE_RANGE_MEX_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

inline std::vector<std::size_t> offline_range_mex(
    const std::vector<std::int64_t>& values,
    const std::vector<std::pair<std::size_t, std::size_t>>& ranges
){
    struct Query{ std::size_t left; std::size_t right; std::size_t index; };
    std::vector<Query> queries;
    queries.reserve(ranges.size());
    for(std::size_t index = 0; index < ranges.size(); index++){
        const auto [left, right] = ranges[index];
        if(left > right || right > values.size()){
            throw std::out_of_range("offline_range_mex: invalid half-open range");
        }
        queries.push_back(Query{left, right, index});
    }
    std::sort(queries.begin(), queries.end(), [](const Query& lhs, const Query& rhs){
        return lhs.right < rhs.right;
    });

    const std::size_t universe = values.size() + 1;
    std::size_t leaf_count = 1;
    while(leaf_count < universe) leaf_count <<= 1;
    std::vector<int> minimum_last(leaf_count << 1, -1);
    const auto set_last = [&](std::size_t position, int last){
        std::size_t node = leaf_count + position;
        minimum_last[node] = last;
        while(node > 1){
            node >>= 1;
            minimum_last[node] = std::min(minimum_last[node << 1], minimum_last[node << 1 | 1]);
        }
    };
    const auto first_before = [&](int boundary){
        std::size_t node = 1;
        while(node < leaf_count){
            if(minimum_last[node << 1] < boundary) node <<= 1;
            else node = node << 1 | 1;
        }
        return node - leaf_count;
    };

    std::vector<std::size_t> answer(ranges.size());
    std::size_t processed = 0;
    for(const Query& query: queries){
        while(processed < query.right){
            const std::int64_t value = values[processed];
            if(value >= 0 && static_cast<std::uint64_t>(value) < universe){
                set_last(static_cast<std::size_t>(value), static_cast<int>(processed));
            }
            ++processed;
        }
        answer[query.index] = first_before(static_cast<int>(query.left));
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_OFFLINE_RANGE_MEX_HPP_INCLUDED
