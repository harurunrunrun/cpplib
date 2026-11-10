---
title: Knuth Optimization (Knuth最適化)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/knuth_optimization.hpp
---

半開区間の DP

$$
dp[l][r]=w(l,r)+\min_{l<k<r}(dp[l][k]+dp[k][r])
$$

を、四辺形不等式から得られる最適な分割位置の単調性を用いて高速化する。

## API

```cpp
template<class Value, class IntervalCost>
KnuthOptimizationResult<Value> knuth_optimization(
    int size,
    IntervalCost interval_cost,
    Value infinity
);
```

$0 \le l \le r \le N$（$N$ は `size`）のすべての区間について、`value[l][r]` と最適な分割位置 `optimum[l][r]` を返す。長さ $0$ と $1$ の区間の値は $0$ で、`interval_cost` は長さ $2$ 以上の区間に対してだけ呼ばれる。$l > r$ の要素は `infinity` と `-1` のままである。

## 時間計算量

`interval_cost` の評価が $O(1)$ なら $O(N^2)$ 時間である。

## 空間計算量

戻り値の $O(N^2)$。

## 注意点

- $w$ が四辺形不等式と区間の包含に関する単調性を満たし、最適な分割位置が `opt[l][r-1] <= opt[l][r] <= opt[l+1][r]` を満たすことを前提とする。
- 負の `size` では `std::runtime_error` を送出する。
