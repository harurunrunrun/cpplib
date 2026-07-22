---
title: Minimum Total Interval Cut Cost (区間切断の最小総費用) [BRKSTRNG]
documentation_of: ../src/algorithm/other/dynamic_programming/minimum_total_interval_cut_cost.hpp
---

一次元区間を指定位置ですべて切断するときの最小総費用を返す。
各操作の費用は、その操作で切断する部分区間の長さである。

## API

```cpp
long long minimum_total_interval_cut_cost(
    long long interval_length, const vector<long long>& cut_positions
);
```

- `interval_length`: 元の区間 $[0, interval\_length]$ の長さ。
- `cut_positions`: 実行する全切断位置を狭義昇順で並べた列。
- 戻り値: 全切断を実行する順序のうち、費用合計の最小値。

## API別の時間計算量・空間計算量

`M = cut_positions.size()` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_total_interval_cut_cost` | $O(M^2)$ | $O(M^2)$ |

## 注意点

- `interval_length > 0` が必要である。
- 各切断位置は $(0, interval\_length)$ 内で、重複なく狭義昇順でなければならない。
- 前提違反時は `std::runtime_error` を送出する。
- 費用合計は `long long` の表現範囲内でなければならない。
