---
title: Complement Shortest Path
documentation_of: ../src/algorithm/graph/complement_shortest_path.hpp
---

補グラフ上の単一始点無重み最短路。

`graph[v]` に元のグラフの辺を入れる。補グラフでは、`v != u` かつ `v -> u` が元のグラフに存在しないときに辺がある。

# 関数

```cpp
complement_shortest_path(graph, source)
```

`ComplementShortestPathResult` は次を持つ。

```cpp
vector<int> dist;
vector<int> parent;
```

到達不能な頂点の `dist` は `-1`。

## 時間計算量

- $O(N^2+M)$
