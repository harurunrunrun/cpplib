#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_RANGE_MODE_QUERIES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_RANGE_MODE_QUERIES_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

// A mode and its multiplicity for each nonempty half-open interval.
template<class T>
std::vector<std::pair<T, int>> range_mode_queries(
    const std::vector<T>& values, const std::vector<std::pair<int, int>>& ranges){
    const int n = static_cast<int>(values.size()), q = static_cast<int>(ranges.size());
    for(auto [l, r]: ranges) if(l < 0 || l >= r || r > n)
        throw std::out_of_range("range mode interval");
    std::vector<T> keys = values;
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    std::vector<int> a(n), order(q), count(keys.size()), head(n + 1, -1), next(keys.size(), -1), prev(keys.size(), -1);
    for(int i = 0; i < n; ++i) a[i] = std::lower_bound(keys.begin(), keys.end(), values[i]) - keys.begin();
    std::iota(order.begin(), order.end(), 0);
    const int block = std::max(1, static_cast<int>(n / std::sqrt(std::max(1, q))));
    std::sort(order.begin(), order.end(), [&](int x, int y){
        const int bx = ranges[x].first / block, by = ranges[y].first / block;
        if(bx != by) return bx < by;
        return bx & 1 ? ranges[x].second > ranges[y].second : ranges[x].second < ranges[y].second;
    });
    int maximum = 0;
    auto change = [&](int x, int delta){
        int f = count[x];
        if(f){
            if(prev[x] == -1) head[f] = next[x]; else next[prev[x]] = next[x];
            if(next[x] != -1) prev[next[x]] = prev[x];
        }
        f = count[x] += delta;
        if(f){
            prev[x] = -1; next[x] = head[f];
            if(head[f] != -1) prev[head[f]] = x;
            head[f] = x;
        }
        maximum = std::max(maximum, f);
        while(maximum > 0 && head[maximum] == -1) --maximum;
    };
    int left = 0, right = 0;
    std::vector<std::pair<T, int>> answer(q);
    for(int id: order){
        const auto [l, r] = ranges[id];
        while(left > l) change(a[--left], 1);
        while(right < r) change(a[right++], 1);
        while(left < l) change(a[left++], -1);
        while(right > r) change(a[--right], -1);
        answer[id] = {keys[head[maximum]], maximum};
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_RANGE_MODE_QUERIES_HPP_INCLUDED
