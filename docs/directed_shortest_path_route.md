---
title: Directed Shortest Path Route (有向グラフ最短路復元) [shortest_path]
documentation_of: ../src/algorithm/graph/directed_shortest_path_route.hpp
---

非負重み有向グラフの2頂点間について、最短距離と最短路を取得する。

# `DirectedShortestPathRouteEdge`

```cpp
template<class T>
struct DirectedShortestPathRouteEdge {
    int from;
    int to;
    T cost;
};
```

`from` から `to` への有向辺と、その非負コストを表す。

# `DirectedShortestPathRouteResult`

```cpp
template<class T>
struct DirectedShortestPathRouteResult {
    bool reachable;
    T distance;
    vector<pair<int, int>> edges;
};
```

到達可能なら `reachable` が真になり、`distance` と始点から終点の順に並んだ
最短路の有向辺列を保持する。到達不能なら `reachable` は偽、`distance` は
`T(0)`、`edges` は空になる。

# `directed_shortest_path_route`

```cpp
template<class T>
DirectedShortestPathRouteResult<T> directed_shortest_path_route(
    int vertex_count,
    const vector<DirectedShortestPathRouteEdge<T>>& edges,
    int source,
    int target
)
```

`source` から `target` への最短距離と最短路を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N+M)\log(N+1))$。

## 空間計算量

入力から構築する隣接リスト、探索領域、戻り値を含めて $O(N+M)$。

## 注意点

- `vertex_count` は正で、全頂点番号は $[0,N)$ でなければならない。
- 辺コストは非負でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 距離の加算結果は `T` で表現できる必要がある。
- 最短路が複数存在する場合、返す経路はそのうち1つである。
