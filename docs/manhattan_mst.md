---
title: Manhattan MST
documentation_of: ../src/algorithm/graph/manhattan_mst.hpp
---

2次元点集合のマンハッタン距離に対する最小全域木を求める。

# 関数

```cpp
manhattan_mst<T>(points)
```

`points` は `ManhattanMstPoint<T>{x, y}` の列。

`ManhattanMstResult<T>` は次を持つ。

```cpp
T cost;
vector<ManhattanMstEdge<T>> edges;
```

## 時間計算量

- $O(N\log N)$
