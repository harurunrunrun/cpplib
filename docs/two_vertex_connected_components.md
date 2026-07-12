---
title: Two Vertex Connected Components
documentation_of: ../src/algorithm/graph/two_vertex_connected_components.hpp
---

無向グラフの二重頂点連結成分分解。

# 関数

```cpp
two_vertex_connected_components(n, edges)
```

`TwoVertexConnectedComponentsResult` は次を持つ。

```cpp
int count;
vector<int> articulation;
vector<vector<int>> vertex_groups;
vector<vector<int>> edge_groups;
```

`edge_groups` には入力辺の id が入る。

孤立点は1頂点の成分として返す。

## 時間計算量

- $O(N+M)$
