---
title: Bellman Ford
documentation_of: ../src/algorithm/graph/bellman_ford.hpp
---

単一始点最短路。負辺に対応する。

# 関数

```cpp
bellman_ford<T>(int n, vector<BellmanFordEdge<T>> edges, int source, T inf)
```

`BellmanFordResult<T>` は次を持つ。

```cpp
vector<T> dist;
vector<int> parent;
vector<char> negative;
bool has_negative_cycle;
```

`negative[v]` は、始点から到達可能な負閉路の影響を受ける頂点を表す。

## 時間計算量

- $O(NM)$
