---
title: 0/1 Knapsack Maximum Value with Minimum Cost (0/1ナップサックの最大価値・最小費用) [PARTY]
documentation_of: ../src/algorithm/other/dynamic_programming/knapsack_max_value_min_cost.hpp
---

`capacity` 以下の合計costで得られる合計valueを最大化し、その最大値を達成する
最小costも返す0/1ナップサック。

## API

```cpp
struct KnapsackItem { int cost, value; };
struct KnapsackOptimum { int minimum_cost, maximum_value; };
KnapsackOptimum knapsack_max_value_min_cost(int capacity, const vector<KnapsackItem>& items);
```

- `KnapsackItem::cost`: itemを選んだときに消費するcost。
- `KnapsackItem::value`: itemを選んだときに得るvalue。
- `KnapsackOptimum::maximum_value`: capacity以下で得られる最大の合計value。
- `KnapsackOptimum::minimum_cost`: `maximum_value` を達成する最小の合計cost。
- `knapsack_max_value_min_cost`: 各itemを高々1回選ぶ最適値を返す。

## API別の時間計算量・空間計算量

`N = items.size()`, `C = capacity` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `knapsack_max_value_min_cost` | $O(NC)$ | $O(C)$ |

## 注意点

- `capacity`, 各itemの `cost`, `value` は非負でなければならない。
- 条件違反では `std::runtime_error` を送出する。
- valueの合計は `int` の表現範囲内でなければならない。
