---
title: Count Vertices Reaching Target Within Limit (制限時間内に目標へ到達できる頂点数) [MICEMAZE]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/specialized/count_vertices_reaching_target_within.hpp
---

非負重みの有向グラフで、指定した距離以内に目標の頂点へ到達できる始点の個数を求める。逆グラフ上で目標から Dijkstra 法を 1 回実行する。

## `ReachingTargetEdge`

```cpp
template <class T>
struct ReachingTargetEdge {
    int from;
    int to;
    T cost;
};
```

有向辺 `from -> to` とその非負のコストを表す。

## `count_vertices_reaching_target_within`

```cpp
template <class T>
int count_vertices_reaching_target_within(
    int vertex_count,
    const std::vector<ReachingTargetEdge<T>>& edges,
    int target,
    const T& limit
);
```

`target` への最短距離が `limit` 以下である頂点の個数を返す。目標の頂点自身も最短距離 $0$ の候補として数える。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N + M)\log(N + M + 1))$。

## 空間計算量

$O(N + M)$

## 注意点

- `vertex_count` は正で、`target` と各辺の端点は $[0, N)$ の範囲内でなければならない。
- 辺のコストは非負でなければならない。
- これらの前提に違反すると `runtime_error` を送出する。
- 距離の加算結果は `T` で表現できなければならない。
