#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_RANGE_INVERSION_QUERIES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_RANGE_INVERSION_QUERIES_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
std::vector<long long> range_inversion_queries(
    const std::vector<T>& values, const std::vector<std::pair<int, int>>& ranges){
    const int n = static_cast<int>(values.size()), q = static_cast<int>(ranges.size());
    for(auto [l, r]: ranges) if(l < 0 || l > r || r > n)
        throw std::out_of_range("range inversion interval");
    std::vector<T> keys = values;
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    const int m = static_cast<int>(keys.size());
    std::vector<int> a(n), order(q), bit(m + 1);
    for(int i = 0; i < n; ++i) a[i] = std::lower_bound(keys.begin(), keys.end(), values[i]) - keys.begin();
    std::iota(order.begin(), order.end(), 0);
    const int block = std::max(1, static_cast<int>(n / std::sqrt(std::max(1, q))));
    std::sort(order.begin(), order.end(), [&](int x, int y){
        int bx = ranges[x].first / block, by = ranges[y].first / block;
        if(bx != by) return bx < by;
        return bx & 1 ? ranges[x].second > ranges[y].second : ranges[x].second < ranges[y].second;
    });
    auto prefix = [&](int x){ int s = 0; for(; x; x -= x & -x) s += bit[x]; return s; };
    auto add = [&](int x, int d){ for(++x; x <= m; x += x & -x) bit[x] += d; };
    int left = 0, right = 0;
    long long inversions = 0;
    std::vector<long long> answer(q);
    for(int id: order){
        const auto [l, r] = ranges[id];
        while(left > l){ int x = a[--left]; inversions += prefix(x); add(x, 1); }
        while(right < r){ int x = a[right]; inversions += right - left - prefix(x + 1); add(x, 1); ++right; }
        while(left < l){ int x = a[left++]; add(x, -1); inversions -= prefix(x); }
        while(right > r){ int x = a[--right]; add(x, -1); inversions -= right - left - prefix(x + 1); }
        answer[id] = inversions;
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_RANGE_INVERSION_QUERIES_HPP_INCLUDED
