---
title: Directed MST
documentation_of: ../src/algorithm/graph/directed_mst.hpp
---

最小有向全域木。rootから全頂点へ到達する有向木の最小コストを求める。

# 関数

```cpp
directed_mst<T>(n, root, edges, inf)
```

`edges` は `DirectedMstEdge<T>{from, to, cost}` の列。

`DirectedMstResult<T>` は次を持つ。

```cpp
T cost;
bool exists;
```

存在しない場合 `exists` は `false`。

## 時間計算量

- $O(NM)$
