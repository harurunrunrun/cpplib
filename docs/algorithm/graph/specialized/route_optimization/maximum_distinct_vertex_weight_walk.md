---
title: Maximum Distinct-vertex-weight Walk (異なる頂点の重み和が最大の歩道) [GOODA]
documentation_of: ../../../../../src/algorithm/graph/specialized/route_optimization/maximum_distinct_vertex_weight_walk.hpp
---

有向グラフの始点から終点までの歩道について、一度でも訪れた頂点の非負重みを 1 回ずつ加えた総和の最大値を求める。

## API

### `maximum_distinct_vertex_weight_walk(graph, vertex_weight, start, goal)`

```cpp
optional<long long> maximum_distinct_vertex_weight_walk(
    const vector<vector<int>>& graph,
    const vector<long long>& vertex_weight,
    int start,
    int goal
);
```

`start` から `goal` へ到達できない場合は `nullopt`、到達できる場合は訪れた相異なる頂点の重み和の最大値を返す。強連結成分ごとに重みをまとめ、縮約した DAG 上の最大重み路として計算する。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_distinct_vertex_weight_walk` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- `vertex_weight.size() == graph.size()` が必要である。
- すべての頂点重みは非負でなければならない。
- `start`, `goal` と全辺の行き先は頂点番号の範囲内でなければならない。
- 前提違反には `invalid_argument` または `out_of_range`、頂点数を `int` で表せない場合は `length_error`、重み和が `long long` に収まらない場合は `overflow_error` を送出する。
