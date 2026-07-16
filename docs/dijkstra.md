---
title: Dijkstra (ダイクストラ法) [GRL_1_A]
documentation_of: ../src/algorithm/graph/dijkstra.hpp
---

単一始点最短路。辺の重みは非負である必要がある。

## 関数

```cpp
dijkstra<T>(graph, source, inf)
```

`graph[v]` は `DijkstraEdge<T>{to, cost}` の列。

`DijkstraResult<T>` は次を持つ。

```cpp
vector<T> dist;
vector<int> parent;
vector<char> reachable;
```

`reachable[v]` は始点から `v` へ到達可能なら真。`inf` は到達不能な頂点の `dist` に格納する初期値であり、到達可能な最短距離が `inf` 以上でも扱える。

距離と辺コストの加算結果は `T` で表現できる必要がある。

## 時間計算量

- `dijkstra`: $O((N+M)\log(N+1))$

全辺の検査、priority queueによる探索、長さ $N$ の結果列構築を含む。

## 空間計算量

- 戻り値とpriority queueを含めて $O(N+M)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
