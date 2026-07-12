#pragma once

#include <algorithm>
#include <vector>

template<class Sequence>
int longest_common_subsequence_length(const Sequence& a, const Sequence& b){
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    std::vector<int> dp(m + 1), next(m + 1);
    for(int i = 0; i < n; i++){
        next[0] = 0;
        for(int j = 0; j < m; j++){
            if(a[i] == b[j]){
                next[j + 1] = dp[j] + 1;
            }else{
                next[j + 1] = std::max(next[j], dp[j + 1]);
            }
        }
        dp.swap(next);
    }
    return dp[m];
}

template<class Sequence>
Sequence longest_common_subsequence(const Sequence& a, const Sequence& b){
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1));
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            if(a[i] == b[j]){
                dp[i + 1][j + 1] = dp[i][j] + 1;
            }else{
                dp[i + 1][j + 1] = std::max(dp[i][j + 1], dp[i + 1][j]);
            }
        }
    }
    Sequence result;
    int i = n, j = m;
    while(i > 0 && j > 0){
        if(a[i - 1] == b[j - 1]){
            result.push_back(a[i - 1]);
            i--;
            j--;
        }else if(dp[i - 1][j] >= dp[i][j - 1]){
            i--;
        }else{
            j--;
        }
    }
    std::reverse(result.begin(), result.end());
    return result;
}
