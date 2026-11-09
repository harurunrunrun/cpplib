---
title: Dijkstra (ダイクストラ法) [GRL_1_A]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/source/dijkstra.hpp
---

単一始点最短路を求める。辺の重みは非負でなければならない。

## 関数

```cpp
template <class T>
DijkstraResult<T> dijkstra(
    const std::vector<std::vector<DijkstraEdge<T>>>& graph,
    int source,
    T inf = std::numeric_limits<T>::max() / 4
)
```

`graph[v]` は `DijkstraEdge<T>{to, cost}` の列で、頂点 `v` から出る辺を表す。

戻り値 `DijkstraResult<T>` は次のメンバをもつ。

```cpp
std::vector<T> dist;
std::vector<int> parent;
std::vector<char> reachable;
```

- `dist[v]`: 始点から `v` への最短距離。到達不能なら `inf`。
- `parent[v]`: 最短路木における `v` の親。始点と到達不能な頂点では `-1`。
- `reachable[v]`: 始点から `v` へ到達可能なら真。

`inf` は到達不能な頂点の `dist` に格納する初期値にすぎず、到達可能な最短距離が `inf` 以上でも正しく扱える。到達判定には `reachable` を使う。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O((N + M) \log (N + M + 1))$。

全辺の検査、優先度付きキューによる探索、長さ $N$ の結果列の構築を含む。

## 空間計算量

戻り値と優先度付きキューを含めて $O(N + M)$。

## 注意点

- `source` や辺の行き先が $[0, N)$ の範囲外、または負の重みの辺がある場合は `runtime_error` を送出する。
- 距離と辺の重みの加算結果は `T` で表現できなければならない。
