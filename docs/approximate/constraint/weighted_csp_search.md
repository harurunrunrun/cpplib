---
title: Weighted CSP Search (重み付きCSP探索)
documentation_of: ../../../src/approximate/constraint/weighted_csp_search.hpp
---

有限領域 CSP を、constraint scope と違反判定 callback で表す。

```cpp
constraint_violated(
    constraint_index,
    std::span<const std::size_t> assignment) -> bool;
```

## Constraint Weighting

```cpp
constraint_weighting(
    domain_sizes, constraint_scopes, constraint_violated,
    random, steps_per_restart, restart_limit,
    weight_increment = 1);
```

ランダムな違反 constraint の scope 内で、weighted violation が最小となる
variable/value move を選ぶ。改善 move がなければ現在違反している constraint
の weight を増やす。

constraint 数を `C`、選択 scope の異なる変数・候補値の合計を `D`、違反判定
1回を `Q` とすると、時間計算量は
`O(restart_limit * steps_per_restart * D * C * Q)`、追加空間計算量は
`O(V+C)`。

## Random Walk CSP

```cpp
random_walk_csp(
    domain_sizes, constraint_scopes, constraint_violated,
    random, steps_per_restart, restart_limit);
```

ランダムな違反 constraint、その scope の変数、domain value を順にランダム
選択する。時間計算量は `O(restart_limit * steps_per_restart * C * Q)`、
追加空間計算量は `O(V+C)`。

## `ConstraintSearchResult`

`assignment`、その assignment での `violated_constraints`、`steps`、
`restarts`、完全充足を表す `found` を返す。

## 注意点

各 domain は非空、scope の変数番号は範囲内でなければならない。空 scope の
違反 constraint は move で修復できない。callback は渡された assignment を
変更してはならない。両手法とも不完全であり、失敗は infeasible の証明ではない。
