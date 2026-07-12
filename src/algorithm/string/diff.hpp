#pragma once

#include <algorithm>
#include <vector>

enum class DiffTag{
    Equal,
    Insert,
    Erase,
};

template<class T>
struct DiffOp{
    DiffTag tag;
    T value;
};

template<class Sequence>
std::vector<DiffOp<typename Sequence::value_type>> diff_sequence(const Sequence& a, const Sequence& b){
    using T = typename Sequence::value_type;
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
    std::vector<DiffOp<T>> result;
    int i = n, j = m;
    while(i > 0 || j > 0){
        if(i > 0 && j > 0 && a[i - 1] == b[j - 1]){
            result.push_back({DiffTag::Equal, a[i - 1]});
            i--;
            j--;
        }else if(j > 0 && (i == 0 || dp[i][j - 1] >= dp[i - 1][j])){
            result.push_back({DiffTag::Insert, b[j - 1]});
            j--;
        }else{
            result.push_back({DiffTag::Erase, a[i - 1]});
            i--;
        }
    }
    std::reverse(result.begin(), result.end());
    return result;
}
