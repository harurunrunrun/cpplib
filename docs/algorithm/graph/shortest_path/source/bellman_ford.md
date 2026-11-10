---
title: Bellman Ford (ベルマン・フォード法) [GRL_1_B]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/source/bellman_ford.hpp
---

単一始点最短路を求める。負の辺を扱え、始点から到達可能な負閉路の影響も検出する。

## 関数

```cpp
template <class T>
BellmanFordResult<T> bellman_ford(
    int n,
    const std::vector<BellmanFordEdge<T>>& edges,
    int source,
    T inf = std::numeric_limits<T>::max() / 4
)
```

`edges` は `BellmanFordEdge<T>{from, to, cost}` の列である。

戻り値 `BellmanFordResult<T>` は次のメンバをもつ。

```cpp
std::vector<T> dist;
std::vector<int> parent;
std::vector<char> reachable;
std::vector<char> negative;
bool has_negative_cycle;
```

- `dist[v]`: 始点から `v` への最短距離。到達不能なら `inf`。`negative[v]` が真の頂点では意味をもたない。
- `parent[v]`: 最短路における `v` の直前の頂点。始点と到達不能な頂点では `-1`。
- `reachable[v]`: 始点から `v` へ到達可能なら真。`inf` は到達不能な頂点の `dist` に格納する初期値にすぎず、到達可能な最短距離が `inf` 以上でも正しく扱える。
- `negative[v]`: 始点から到達可能な負閉路の影響を受け、最短距離が $-\infty$ になる頂点なら真。
- `has_negative_cycle`: `negative` に真の要素が 1 つでもあれば真。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O(NM + N)$。

最短距離の緩和と負閉路の影響の伝播をそれぞれ高々 $N$ 回行い、長さ $N$ の各結果列を構築する。

## 空間計算量

戻り値を含めて $O(N)$（入力を除く）。

## 注意点

- `n` が負、`source` や辺の端点が $[0, n)$ の範囲外なら `runtime_error` を送出する。
- 距離と辺の重みの加算結果は `T` で表現できなければならない。
