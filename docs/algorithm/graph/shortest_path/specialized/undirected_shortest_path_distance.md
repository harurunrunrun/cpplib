---
title: Undirected Shortest Path Distance (無向グラフ二頂点間最短距離) [HIGHWAYS]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/specialized/undirected_shortest_path_distance.hpp
---

非負重みの無向グラフで、指定した 2 頂点間の最短距離を Dijkstra 法で求める。

## `UndirectedShortestPathEdge`

```cpp
template <class T>
struct UndirectedShortestPathEdge {
    int left;
    int right;
    T cost;
};
```

無向辺とその非負のコストを表す。

## `undirected_shortest_path_distance`

```cpp
template <class T>
std::optional<T> undirected_shortest_path_distance(
    int vertex_count,
    const std::vector<UndirectedShortestPathEdge<T>>& edges,
    int source,
    int target
);
```

`source` から `target` までの最短距離を返す。到達不能なら `std::nullopt` を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N + M)\log(N + M + 1))$。

## 空間計算量

$O(N + M)$

## 注意点

- `vertex_count` は正で、`source`, `target` と辺の端点は $[0, N)$ の範囲内でなければならない。
- 辺のコストは非負でなければならない。
- これらの前提に違反すると `runtime_error` を送出する。
- 距離の加算結果は `T` で表現できなければならない。
