---
title: Maximum Distinct-vertex-weight Walk (異なる頂点重み和最大walk) [GOODA]
documentation_of: ../src/algorithm/graph/maximum_distinct_vertex_weight_walk.hpp
---

有向グラフの始点から終点までのwalkについて、一度でも訪れた頂点の非負重みを
1回ずつ加えた総和の最大値を求めます。

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

`start` から `goal` へ到達できない場合は `nullopt`、到達できる場合は
異なる訪問頂点の重み和の最大値を返します。強連結成分ごとに重みをまとめ、
縮約DAG上の最大重み路として計算します。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_distinct_vertex_weight_walk` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- `vertex_weight.size() == graph.size()` が必要です。
- すべての頂点重みは非負でなければなりません。
- `start`, `goal` と全辺の終点は頂点範囲内でなければなりません。
- 前提違反には `invalid_argument` または `out_of_range`、内部添字を
  `int` で表せない場合は `length_error`、重み和が `long long` を
  超える場合は `overflow_error` を送出します。
