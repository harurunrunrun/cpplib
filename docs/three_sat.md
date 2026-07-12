---
title: Three Sat
documentation_of: ../src/algorithm/graph/three_sat.hpp
---

3-SAT。

# 関数

```cpp
three_sat(n, clauses)
```

`clauses` は `ThreeSatClause` の列。各節は3つの `ThreeSatLiteral{variable, value}` を持つ。

戻り値 `ThreeSatResult` は次を持つ。

```cpp
bool satisfiable;
vector<int> assignment;
```

充足不能なら `assignment` は空。

## 時間計算量

指数時間。
