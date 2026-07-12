---
title: Kruskal MST
documentation_of: ../src/algorithm/graph/kruskal_mst.hpp
---

Kruskal法で最小全域木を求める。

# 関数

```cpp
kruskal_mst<T>(n, edges)
```

`edges` は `KruskalEdge<T>{from, to, cost}` の列。

`KruskalMstResult<T>` は次を持つ。

```cpp
T cost;
vector<int> edge_ids;
bool connected;
```

非連結なら最小全域森を返し、`connected` は `false`。

## 時間計算量

- $O(M\log M)$
