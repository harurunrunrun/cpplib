---
title: Maximum Weight of Non-overlapping Intervals (互いに重ならない区間の最大重み) [RENT]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/sequence_selection/maximum_weight_nonoverlapping_intervals.hpp
---

重み付きの半開区間の集合から互いに重ならない区間を選び、重みの総和を最大化する（区間スケジューリングの重み付き版）。終点でソートし、二分探索を用いた DP で計算する。

## `WeightedInterval`

```cpp
struct WeightedInterval {
    long long begin;
    long long end;
    long long weight;
};
```

半開区間 $[\mathrm{begin},\mathrm{end})$ と非負の重みを保持する。

## `maximum_weight_nonoverlapping_intervals`

```cpp
long long maximum_weight_nonoverlapping_intervals(
    std::vector<WeightedInterval> intervals
);
```

選んだ区間の重みの総和の最大値を返す。ある区間の `end` と別の区間の `begin` が等しい場合、それらは重ならないとみなす。空の入力には $0$ を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_weight_nonoverlapping_intervals` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 各区間は `begin < end`、`weight >= 0` を満たす必要がある。
- 前提に違反した場合、または答えが `long long` に収まらない場合は `std::runtime_error` を送出する。
