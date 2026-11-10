---
title: Minimum Total Interval Cut Cost (区間切断の最小総費用) [BRKSTRNG]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/minimum_total_interval_cut_cost.hpp
---

一次元の区間を指定した位置ですべて切断するときの、最小の総費用を返す。
各操作の費用は、その操作で切断する部分区間の長さである。Knuth 最適化を用いた区間 DP で計算する。

## API

```cpp
long long minimum_total_interval_cut_cost(
    long long interval_length, const std::vector<long long>& cut_positions
);
```

- `interval_length`: 元の区間 $[0, L]$ の長さ $L$。
- `cut_positions`: 実行するすべての切断位置を狭義単調増加に並べた列。
- 戻り値: すべての切断を実行する順序を選んだときの、費用の合計の最小値。

## API別の時間計算量・空間計算量

$M$ を `cut_positions.size()` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_total_interval_cut_cost` | $O(M^2)$ | $O(M^2)$ |

## 注意点

- `interval_length > 0` が必要である。
- 各切断位置は $(0, L)$ 内で、重複なく狭義単調増加に並んでいなければならない。
- 前提違反時は `std::runtime_error` を送出する。
- 費用の合計は `long long` の範囲に収まらなければならない（オーバーフローは検査しない）。
