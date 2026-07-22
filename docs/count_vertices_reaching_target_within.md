---
title: Count Vertices Reaching Target Within Limit (制限時間内に目標へ到達できる頂点数) [MICEMAZE]
documentation_of: ../src/algorithm/graph/shortest_path/count_vertices_reaching_target_within.hpp
---

非負重み有向グラフで、指定した距離以内に目標頂点へ到達できる始点の個数を求める。

## ReachingTargetEdge

```cpp
template<class T>
struct ReachingTargetEdge {
    int from;
    int to;
    T cost;
};
```

有向辺 `from -> to` とその非負コストを表す。

## count_vertices_reaching_target_within

```cpp
template<class T>
int count_vertices_reaching_target_within(
    int vertex_count,
    const vector<ReachingTargetEdge<T>>& edges,
    int target,
    const T& limit
);
```

最短距離が `limit` 以下となる始点の個数を返す。目標頂点自身も最短距離 $0$ の候補に含む。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N+M)\log(N+1))$。

## 空間計算量

$O(N+M)$。

## 注意点

- `vertex_count` は正、`target` と各辺端点は $[0,N)$ でなければならない。
- 辺コストは非負でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 距離の加算結果は `T` で表現できる必要がある。
