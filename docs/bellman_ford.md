---
title: Bellman Ford (ベルマン・フォード法) [GRL_1_B]
documentation_of: ../src/algorithm/graph/shortest_path/bellman_ford.hpp
---

単一始点最短路。負辺に対応する。

## 関数

```cpp
bellman_ford<T>(int n, vector<BellmanFordEdge<T>> edges, int source, T inf)
```

`BellmanFordResult<T>` は次を持つ。

```cpp
vector<T> dist;
vector<int> parent;
vector<char> reachable;
vector<char> negative;
bool has_negative_cycle;
```

`reachable[v]` は始点から `v` へ到達可能なら真。`inf` は到達不能な頂点の `dist` に格納する初期値であり、到達可能な最短距離が `inf` 以上でも扱える。

`negative[v]` は、始点から到達可能な負閉路の影響を受ける頂点を表す。

距離と辺コストの加算結果は `T` で表現できる必要がある。

## 時間計算量

- `bellman_ford`: $O(NM+N)$

最短距離の緩和と負閉路影響の伝播をそれぞれ高々 $N$ 回行い、長さ $N$ の各結果列を構築する。

## 空間計算量

- 戻り値を含めて $O(N)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
