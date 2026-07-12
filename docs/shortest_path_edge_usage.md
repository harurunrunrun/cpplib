---
title: Shortest Path Edge Usage
documentation_of: ../src/algorithm/graph/shortest_path_edge_usage.hpp
---

各辺が `source` から `target` への最短路のいずれかに含まれるかを判定する。

辺の重みは非負である必要がある。

# 関数

```cpp
shortest_path_edge_usage<T>(n, edges, source, target, inf)
```

`edges` は入力順の `ShortestPathEdgeUsageEdge<T>{from, to, cost}`。

`ShortestPathEdgeUsageResult<T>` は次を持つ。

```cpp
T shortest;
vector<char> used;
vector<char> unused;
```

`used[i]` は辺 `i` が最短路に含まれることを表す。

## 時間計算量

- $O((N+M)\log N)$
