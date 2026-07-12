---
title: Dijkstra
documentation_of: ../src/algorithm/graph/dijkstra.hpp
---

単一始点最短路。辺の重みは非負である必要がある。

# 関数

```cpp
dijkstra<T>(graph, source, inf)
```

`graph[v]` は `DijkstraEdge<T>{to, cost}` の列。

`DijkstraResult<T>` は次を持つ。

```cpp
vector<T> dist;
vector<int> parent;
```

## 時間計算量

- $O((N+M)\log N)$
