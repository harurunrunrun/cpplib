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

- $O(N+M)$
