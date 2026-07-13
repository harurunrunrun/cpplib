---
title: Two Sat
documentation_of: ../src/algorithm/graph/two_sat.hpp
---

2-SAT。

# コンストラクタ

```cpp
TwoSat ts(n)
```

# 関数

```cpp
ts.add_clause(i, f, j, g)
ts.satisfiable()
```

`add_clause(i, f, j, g)` は `(x_i == f) OR (x_j == g)` を追加する。

`satisfiable()` が `true` の場合、`ts.answer` に割当が入る。

## 時間計算量

$N$ を変数数、$M$ を現在の含意辺数とする。

- `TwoSat(n)`: $O(N)$
- `id`: $O(1)$
- `add_implication`: 償却 $O(1)$
- `add_clause`: 償却 $O(1)$（含意辺を2本追加）
- `satisfiable`: $O(N+M)$

`satisfiable` はSCC分解と長さ $N$ の `answer` 更新を含む。

## 空間計算量

- 含意グラフと割当を含めて $O(N+M)$
