---
title: Two Edge Connected Components
documentation_of: ../src/algorithm/graph/two_edge_connected_components.hpp
---

無向グラフの二重辺連結成分分解。

橋を取り除いた後の連結成分を求める。

# 関数

```cpp
two_edge_connected_components(n, edges)
```

`TwoEdgeConnectedComponentsResult` は次を持つ。

```cpp
int count;
vector<int> id;
vector<vector<int>> groups;
vector<int> bridges;
```

`bridges` には入力辺の id が入る。

## 時間計算量

- $O(N+M)$
