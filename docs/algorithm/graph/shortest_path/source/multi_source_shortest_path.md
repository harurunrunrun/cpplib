---
title: Multi Source Shortest Path (多始点最短路)
documentation_of: ../../../../../src/algorithm/graph/shortest_path/source/multi_source_shortest_path.hpp
---

複数の始点のうち最も近いものからの最短距離を、Dijkstra 法で一度に求める。辺の重みは非負でなければならない。

## 関数

```cpp
template <class T>
MultiSourceShortestPathResult<T> multi_source_shortest_path(
    const std::vector<std::vector<MultiSourceShortestPathEdge<T>>>& graph,
    const std::vector<int>& sources,
    T inf = std::numeric_limits<T>::max() / 4
)
```

`graph[v]` は `MultiSourceShortestPathEdge<T>{to, cost}` の列で、頂点 `v` から出る辺を表す。`sources` に同じ頂点が重複していてもよい。

戻り値 `MultiSourceShortestPathResult<T>` は次のメンバをもつ。

```cpp
std::vector<T> dist;
std::vector<int> source;
std::vector<int> parent;
std::vector<char> reachable;
```

- `dist[v]`: いずれかの始点から `v` への最短距離。到達不能なら `inf`。
- `source[v]`: 最短路の始点。到達不能なら `-1`。
- `parent[v]`: 最短路における `v` の直前の頂点。始点と到達不能な頂点では `-1`。
- `reachable[v]`: いずれかの始点から `v` へ到達可能なら真。`inf` は到達不能な頂点の `dist` に格納する初期値にすぎず、到達可能な最短距離が `inf` 以上でも正しく扱える。

## 時間計算量

頂点数を $N$、辺数を $M$、入力した始点の個数を $S$ として $O(S + (N + M)\log(N + M + 1))$。

始点と全辺の検査、Dijkstra 法、長さ $N$ の結果列の構築を含む。

## 空間計算量

戻り値と優先度付きキューを含めて $O(N + M)$。

## 注意点

- 始点や辺の行き先が $[0, N)$ の範囲外、または負の重みの辺がある場合は `runtime_error` を送出する。
- 距離と辺の重みの加算結果は `T` で表現できなければならない。
