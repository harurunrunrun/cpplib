---
title: Weighted Grid Shortest Path (重み付き格子最短路) [BYTESE1]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/specialized/weighted_grid_shortest_path.hpp
---

非負の頂点コストをもつ長方形の格子上で、4 近傍の移動による最短路のコストを Dijkstra 法で求める。

## `weighted_grid_shortest_path`

```cpp
long long weighted_grid_shortest_path(
    const std::vector<std::vector<long long>>& cost,
    int start_row,
    int start_column,
    int target_row,
    int target_column
);
```

## 引数

- `cost`: 各マスに入るときに加算する非負のコスト。
- `start_row`, `start_column`: 始点のマス（0-indexed）。
- `target_row`, `target_column`: 目標のマス（0-indexed）。

## 戻り値

始点から目標まで 4 近傍で移動する最小コスト。始点と目標を含め、通る各マスの `cost` を 1 回ずつ加算する。

## 計算量

マスの個数を $V$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `weighted_grid_shortest_path` | $O(V\log(V + 1))$ | $O(V)$ |

## 注意点

- `cost` が空、先頭行が空、行の長さが不揃い、または負のコストを含む場合は `invalid_argument` を送出する。
- 始点または目標が格子外なら `out_of_range` を送出する。
- 行数・列数が `int` に収まらない場合や、マスの個数が `std::size_t` に収まらない場合は `length_error` を送出する。
- 内部の距離は 128 ビット整数で計算し、最短距離が `long long` に収まらない場合は `overflow_error` を送出する。
