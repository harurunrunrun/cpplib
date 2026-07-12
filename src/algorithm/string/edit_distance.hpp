#pragma once

#include <algorithm>
#include <vector>

template<class Sequence>
int edit_distance(
    const Sequence& a,
    const Sequence& b,
    int insert_cost = 1,
    int erase_cost = 1,
    int replace_cost = 1
){
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    std::vector<int> dp(m + 1), next(m + 1);
    for(int j = 0; j <= m; j++) dp[j] = j * insert_cost;
    for(int i = 0; i < n; i++){
        next[0] = (i + 1) * erase_cost;
        for(int j = 0; j < m; j++){
            int best = std::min(dp[j + 1] + erase_cost, next[j] + insert_cost);
            best = std::min(best, dp[j] + (a[i] == b[j] ? 0 : replace_cost));
            next[j + 1] = best;
        }
        dp.swap(next);
    }
    return dp[m];
}
