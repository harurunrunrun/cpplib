---
title: Minimum Weighted Left-Endpoint Partition Cost (重み付き左端分割費用最小化) [NKLEAVES]
documentation_of: ../src/algorithm/other/minimum_weighted_left_endpoint_partition_cost.hpp
---

正の重みを持つ一次元列を空でない連続区間へ分割し、各区間の要素をその左端へ移す重み付き距離の総和を最小化する。

## `minimum_weighted_left_endpoint_partition_cost`

```cpp
long long minimum_weighted_left_endpoint_partition_cost(
    const vector<long long>& weights,
    int group_count
);
```

列をちょうど `group_count` 個の空でない連続区間へ分割する。区間 $[l,r)$ の費用を
$\sum_{i=l}^{r-1}\mathtt{weights}[i](i-l)$ とし、全区間の費用合計の最小値を返す。

## API別の時間計算量・空間計算量

要素数を $N$、区間数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_weighted_left_endpoint_partition_cost` | $O(KN)$ | $O(N)$ |

## 注意点

- 全重みは正で、$1\le K\le N$ でなければならない。
- 要素数を内部添字で表せない場合は `length_error`、入力前提違反は
  `runtime_error`、結果が `long long` に収まらない場合は `overflow_error` を送出する。
