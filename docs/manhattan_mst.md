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

- `manhattan_mst`: $O(N\log(N+1))$

4方向のsweep、$O(N)$ 本の候補辺整列、DSU処理、最大 $N-1$ 本の辺出力を含む。

## 空間計算量

- 候補辺、DSU、戻り値を含めて $O(N)$
