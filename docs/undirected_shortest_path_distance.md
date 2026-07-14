---
title: Undirected Shortest Path Distance (無向グラフ二頂点間最短距離) [HIGHWAYS]
documentation_of: ../src/algorithm/graph/undirected_shortest_path_distance.hpp
---

非負重み無向グラフの指定した2頂点間の最短距離を求める。

# UndirectedShortestPathEdge

```cpp
template<class T>
struct UndirectedShortestPathEdge {
    int left;
    int right;
    T cost;
};
```

無向辺とその非負コストを表す。

# undirected_shortest_path_distance

```cpp
template<class T>
optional<T> undirected_shortest_path_distance(
    int vertex_count,
    const vector<UndirectedShortestPathEdge<T>>& edges,
    int source,
    int target
);
```

`source` から `target` までの最短距離を返す。到達不能なら `nullopt` を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N+M)\log(N+1))$。

## 空間計算量

$O(N+M)$。

## 注意点

- `vertex_count` は正で、頂点番号は $[0,N)$ でなければならない。
- 辺コストは非負でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 距離の加算結果は `T` で表現できる必要がある。
