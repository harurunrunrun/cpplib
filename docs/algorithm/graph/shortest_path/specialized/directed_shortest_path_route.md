---
title: Directed Shortest Path Route (有向グラフ最短路復元) [shortest_path]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/specialized/directed_shortest_path_route.hpp
---

非負重みの有向グラフの 2 頂点間について、最短距離と最短路を Dijkstra 法で求める。

## `DirectedShortestPathRouteEdge`

```cpp
template <class T>
struct DirectedShortestPathRouteEdge {
    int from;
    int to;
    T cost;
};
```

`from` から `to` への有向辺と、その非負のコストを表す。

## `DirectedShortestPathRouteResult`

```cpp
template <class T>
struct DirectedShortestPathRouteResult {
    bool reachable;
    T distance;
    std::vector<std::pair<int, int>> edges;
};
```

到達可能なら `reachable` が真になり、`distance` に最短距離を、`edges` に始点から終点の順に並んだ最短路の有向辺 `(from, to)` の列を保持する。到達不能なら `reachable` は偽、`distance` は `T(0)`、`edges` は空になる。`source == target` なら `distance` は $0$、`edges` は空である。

## `directed_shortest_path_route`

```cpp
template <class T>
DirectedShortestPathRouteResult<T> directed_shortest_path_route(
    int vertex_count,
    const std::vector<DirectedShortestPathRouteEdge<T>>& edges,
    int source,
    int target
)
```

`source` から `target` への最短距離と最短路を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N + M)\log(N + M + 1))$。

## 空間計算量

入力から構築する隣接リスト、探索領域、戻り値を含めて $O(N + M)$。

## 注意点

- `vertex_count` は正で、`source`, `target` と辺の端点は $[0, N)$ の範囲内でなければならない。
- 辺のコストは非負でなければならない。
- これらの前提に違反すると `runtime_error` を送出する。
- 距離の加算結果は `T` で表現できなければならない。
- 最短路が複数存在する場合は、そのうちの 1 つを返す。
