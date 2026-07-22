#ifndef CPPLIB_SRC_ALGORITHM_RANGE_MAXIMUM_DISTINCT_SUBARRAY_SUMS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_MAXIMUM_DISTINCT_SUBARRAY_SUMS_HPP_INCLUDED

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../structure/segtree/lazysegtree.hpp"
#include "../../structure/types/common_monoids.hpp"

template<class T, int MAX_SIZE>
std::vector<T> maximum_distinct_subarray_sums(
    const std::vector<T>& values,
    const std::vector<std::pair<int, int>>& ranges
){
    struct Query{ int left; int right; int index; };
    const int size = static_cast<int>(values.size());
    std::vector<Query> queries;
    queries.reserve(ranges.size());
    for(int index = 0; index < static_cast<int>(ranges.size()); ++index){
        const auto [left, right] = ranges[static_cast<std::size_t>(index)];
        if(left < 0 || right <= left || size < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid range "
                "(maximum_distinct_subarray_sums)."
            );
        }
        queries.push_back({left, right - 1, index});
    }
    std::sort(queries.begin(), queries.end(), [](const Query& a, const Query& b){
        return a.right < b.right;
    });
    std::vector<HistoricalMaxAggregate<T>> initial(
        values.size(),
        {T{}, T{}}
    );
    constexpr AddHistoricalMaxMonoidAct<T> action{};
    auto tree = std::make_unique<LazySegtree<action, MAX_SIZE>>(initial);
    std::unordered_map<T, int> last;
    last.reserve(values.size() * 2);
    std::vector<T> answer(ranges.size());
    int processed = 0;
    for(const Query& query: queries){
        while(processed <= query.right){
            const T& value = values[static_cast<std::size_t>(processed)];
            const auto iterator = last.find(value);
            const int previous = iterator == last.end() ? -1 : iterator->second;
            tree->apply(previous + 1, processed + 1, historical_add(value));
            last[value] = processed++;
        }
        answer[static_cast<std::size_t>(query.index)] =
            tree->prod(query.left, query.right + 1).historical;
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_MAXIMUM_DISTINCT_SUBARRAY_SUMS_HPP_INCLUDED
