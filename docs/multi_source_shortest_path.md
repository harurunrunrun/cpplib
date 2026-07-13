---
title: Multi Source Shortest Path
documentation_of: ../src/algorithm/graph/multi_source_shortest_path.hpp
---

複数始点からの単一最短路。辺の重みは非負である必要がある。

# 関数

```cpp
multi_source_shortest_path<T>(graph, sources, inf)
```

`graph[v]` は `MultiSourceShortestPathEdge<T>{to, cost}` の列。

`MultiSourceShortestPathResult<T>` は次を持つ。

```cpp
vector<T> dist;
vector<int> source;
vector<int> parent;
vector<char> reachable;
```

`source[v]` は最短路の始点。到達不能なら `-1`。

`reachable[v]` はいずれかの始点から `v` へ到達可能なら真。`inf` は到達不能な頂点の `dist` に格納する初期値であり、到達可能な最短距離が `inf` 以上でも扱える。

距離と辺コストの加算結果は `T` で表現できる必要がある。

## 時間計算量

$S$ を入力した始点数とする。

- `multi_source_shortest_path`: $O(S+(N+M)\log(N+1))$

始点検査、全辺検査、Dijkstra、長さ $N$ の結果列構築を含む。

## 空間計算量

- 戻り値とpriority queueを含めて $O(N+M)$
