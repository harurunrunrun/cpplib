---
title: 2-SAT Assignment (2-SAT充足割当) [two_sat]
documentation_of: ../../../../../src/algorithm/graph/constraints/logical/two_sat_assignment.hpp
---

構築済みの 2-SAT 制約について、充足可能性と充足割当を 1 つの戻り値で取得する。

## `two_sat_assignment`

```cpp
std::optional<std::vector<int>> two_sat_assignment(TwoSat& solver)
```

`solver` に追加された全制約の充足可能性を判定する。充足可能なら変数番号順の 0/1 割当を返し、充足不可能なら `std::nullopt` を返す。

## 時間計算量

変数の個数を $N$、含意辺の本数を $M$ として $O(N + M)$。戻り値への割当のコピー $O(N)$ を含む。

## 空間計算量

強連結成分分解の作業領域と戻り値を含めて $O(N + M)$。

## 注意点

- 返される列の長さは `solver.n` である。
- 内部で `solver.satisfiable()` を実行するため、`solver.answer` も更新される。
- 変数範囲など `TwoSat` の契約に違反すると `runtime_error` を送出する。
