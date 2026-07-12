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
```

`source[v]` は最短路の始点。到達不能なら `-1`。

## 時間計算量

- $O((N+M)\log N)$
