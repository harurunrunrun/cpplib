---
title: Minimum Weighted Left-Endpoint Partition Cost (重み付き左端分割費用最小化) [NKLEAVES]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/minimum_weighted_left_endpoint_partition_cost.hpp
---

正の重みをもつ一次元の列を空でない連続区間に分割し、各区間の要素をその左端に移すときの重み付き距離の総和を最小化する。区間数ごとの DP を Convex Hull Trick で高速化する。

## `minimum_weighted_left_endpoint_partition_cost`

```cpp
long long minimum_weighted_left_endpoint_partition_cost(
    const std::vector<long long>& weights,
    int group_count
);
```

列をちょうど `group_count` 個の空でない連続区間に分割する。$w_i$ を `weights[i]` として区間 $[l,r)$ の費用を $\sum_{i=l}^{r-1} w_i (i-l)$ とし、全区間の費用の合計の最小値を返す。

## API別の時間計算量・空間計算量

要素数を $N$、区間数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_weighted_left_endpoint_partition_cost` | $O(KN)$ | $O(N)$ |

## 注意点

- すべての重みは正で、$1\le K\le N$ でなければならない。
- 要素数が `int` の範囲を超える場合は `std::length_error`、入力の前提違反は `std::runtime_error`、結果が `long long` に収まらない場合は `std::overflow_error` を送出する。
