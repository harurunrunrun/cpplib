---
title: Burn Bury
documentation_of: ../src/algorithm/matching/burn_bury.hpp
---

燃やす埋める。

二値変数のsubmodularなコストを最小カットで最小化する。

# コンストラクタ

```cpp
BurnBury<T> graph(n)
```

# 関数

```cpp
graph.add_one_cost(v, zero_cost, one_cost)
graph.add_pair_cost(i, j, cost00, cost01, cost10, cost11)
graph.solve()
```

`add_pair_cost` は

```text
cost00 + cost11 <= cost01 + cost10
```

を満たす必要がある。

`BurnBuryResult<T>` は次を持つ。

```cpp
T cost;
vector<int> assignment;
```

`assignment[v]` は `0` または `1`。

## 時間計算量

最大流に依存する。
