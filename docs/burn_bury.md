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

$V'=N+2$ をflow networkの頂点数、$E'$ を追加済みflow辺数とする。

- `BurnBury(n)`: $O(N)$
- `add_one_delta`, `add_one_cost`: 償却 $O(1)$
- `add_pair_cost`: 償却 $O(1)$（flow辺を高々3本追加）
- `solve`: Dinic法 $O(V'^2E')$、min-cutと長さ $N$ の割当出力 $O(V'+E')$

## 空間計算量

- flow graph、min-cut、戻り値を含めて $O(V'+E')$
