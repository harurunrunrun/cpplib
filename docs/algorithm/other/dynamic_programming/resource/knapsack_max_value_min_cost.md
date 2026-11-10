---
title: 0/1 Knapsack Maximum Value with Minimum Cost (0/1ナップサックの最大価値・最小費用) [PARTY]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/resource/knapsack_max_value_min_cost.hpp
---

合計コストが `capacity` 以下という条件で合計価値を最大化し、その最大値を達成する最小の合計コストもあわせて返す 0/1 ナップサック。

## API

```cpp
struct KnapsackItem { int cost, value; };
struct KnapsackOptimum { int minimum_cost, maximum_value; };
KnapsackOptimum knapsack_max_value_min_cost(int capacity, const std::vector<KnapsackItem>& items);
```

- `KnapsackItem::cost`: 品物を選んだときに消費するコスト。
- `KnapsackItem::value`: 品物を選んだときに得る価値。
- `KnapsackOptimum::maximum_value`: 合計コスト `capacity` 以下で得られる合計価値の最大値。
- `KnapsackOptimum::minimum_cost`: `maximum_value` を達成する合計コストの最小値。
- `knapsack_max_value_min_cost`: 各品物を高々 1 回選ぶときの上記の最適値を返す。

## API別の時間計算量・空間計算量

$N$ を `items.size()`、$C$ を `capacity` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `knapsack_max_value_min_cost` | $O(NC)$ | $O(C)$ |

## 注意点

- `capacity` と各品物の `cost`, `value` は非負でなければならない。
- 条件に違反した場合は `std::runtime_error` を送出する。
- 価値の合計は `int` の範囲に収まらなければならない（オーバーフローは検査しない）。
