---
title: Prim MST
documentation_of: ../src/algorithm/graph/prim_mst.hpp
---

Prim法で最小全域木を求める。

# 関数

```cpp
prim_mst<T>(graph)
```

`graph[v]` は `PrimEdge<T>{to, cost}` の列。無向グラフでは両方向に辺を入れる。

`PrimMstResult<T>` は次を持つ。

```cpp
T cost;
vector<PrimTreeEdge<T>> edges;
bool connected;
```

非連結なら最小全域森を返し、`connected` は `false`。

## 時間計算量

- $O(M\log M)$
