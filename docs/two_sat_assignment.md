---
title: 2-SAT Assignment (2-SAT充足割当) [two_sat]
documentation_of: ../src/algorithm/graph/constraints/two_sat_assignment.hpp
---

構築済みの2-SAT制約について、充足可能性と充足割当を1つの返り値で取得する。

## `two_sat_assignment`

```cpp
optional<vector<int>> two_sat_assignment(TwoSat& solver)
```

`solver` の全制約を判定する。充足可能なら変数番号順の0/1割当を返し、
充足不可能なら `nullopt` を返す。

## 時間計算量

変数数を $N$、含意辺数を $M$ として $O(N+M)$。
返り値への割当copy $O(N)$ を含む。

## 空間計算量

SCCの作業領域と返り値を含めて $O(N+M)$。

## 注意点

- 返されたvectorの長さは `solver.n`。
- `solver.satisfiable()` を実行するため、`solver.answer` も更新される。
- 変数範囲など `TwoSat` の契約違反では `runtime_error`。
