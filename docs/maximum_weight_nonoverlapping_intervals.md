---
title: Maximum Weight of Non-overlapping Intervals (互いに重ならない区間の最大重み) [RENT]
documentation_of: ../src/algorithm/other/maximum_weight_nonoverlapping_intervals.hpp
---

重み付き半開区間から互いに重ならない区間を選び、重みの総和を最大化する。

## `WeightedInterval`

```cpp
struct WeightedInterval { long long begin, end, weight; };
```

半開区間 $[\mathrm{begin},\mathrm{end})$ と非負の重みを保持する。

## `maximum_weight_nonoverlapping_intervals`

```cpp
long long maximum_weight_nonoverlapping_intervals(
    vector<WeightedInterval> intervals
);
```

選択した区間の重みの総和の最大値を返す。ある区間の `end` と別の区間の
`begin` が等しい場合、それらは重ならない。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_weight_nonoverlapping_intervals` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 各区間は `begin < end`、`weight >= 0` を満たす必要がある。
- 前提違反または答えが `long long` に収まらない場合は `runtime_error` を送出する。
