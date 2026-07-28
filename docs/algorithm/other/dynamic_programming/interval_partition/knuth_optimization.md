---
title: Knuth Optimization (Knuth最適化)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/knuth_optimization.hpp
---

半開区間DP

$$
dp[l][r]=w(l,r)+\min_{l<k<r}(dp[l][k]+dp[k][r])
$$

を、四辺形不等式から得られる最適分割位置の単調性で高速化する。

## API

```cpp
template<class Value, class IntervalCost>
KnuthOptimizationResult<Value> knuth_optimization(
    int size,
    IntervalCost interval_cost,
    Value infinity
);
```

全区間の `value[l][r]` と `optimum[l][r]` を返す。長さ `0` と `1` の費用は `0`。

## 時間計算量

- `interval_cost` 評価が $O(1)$ の場合: $O(N^2)$ 時間
- 戻り値: $O(N^2)$ 空間

## 注意点

$w$ が四辺形不等式と区間包含に関する単調性を満たし、最適位置が `opt[l][r-1] <= opt[l][r] <= opt[l+1][r]` となることを前提とする。負の `size` では `runtime_error`。
