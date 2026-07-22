---
title: Weighted Grid Shortest Path (重み付き格子最短路) [BYTESE1]
documentation_of: ../src/algorithm/graph/shortest_path/weighted_grid_shortest_path.hpp
---

非負の頂点費用を持つ長方形格子上で、4近傍の最短路費用を求める。

## `weighted_grid_shortest_path`

```cpp
long long weighted_grid_shortest_path(
    const vector<vector<long long>>& cost,
    int start_row,
    int start_column,
    int target_row,
    int target_column
);
```

## 引数

- `cost`: 各マスへ入るときに加算する非負費用。
- `start_row`, `start_column`: 0-indexedの始点。
- `target_row`, `target_column`: 0-indexedの目標マス。

## 戻り値

始点から目標まで4近傍で移動する最小費用。始点と目標を含め、通る各マスの
`cost` を1回ずつ加算する。

## API別の時間計算量・空間計算量

マス数を $V$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `weighted_grid_shortest_path` | $O(V\log(V+1))$ | $O(V)$ |

## 注意点

- `cost` は空でない長方形で、各費用は非負、始点と目標は範囲内で
  なければならない。
- 前提違反、マス数の積、または最短距離が `long long` に収まらない場合は
  対応する標準例外を送出する。
