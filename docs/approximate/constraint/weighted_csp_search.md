---
title: Weighted CSP Search (重み付きCSP探索)
documentation_of: ../../../src/approximate/constraint/weighted_csp_search.hpp
---

有限領域の CSP を、制約ごとの変数集合（スコープ）`constraint_scopes` と違反判定コールバックで表す。

```cpp
constraint_violated(
    constraint_index,
    std::span<const std::size_t> assignment) -> bool;
```

以下、変数数を $V$、制約数を $C$、選んだスコープ内の相異なる変数の候補値の個数の合計を $D$、違反判定 1 回のコストを $Q$ とする。また、`restart_limit` を $R$、`steps_per_restart` を $S$ とする。

## Constraint Weighting

```cpp
constraint_weighting(
    domain_sizes, constraint_scopes, constraint_violated,
    random, steps_per_restart, restart_limit,
    weight_increment = 1);
```

ランダムに選んだ違反制約のスコープ内で、重み付き違反数が最小となる「変数と値」の変更を選ぶ。改善する変更がなければ、現在違反している制約の重みを増やす。

時間計算量は $O(RSDCQ)$、追加空間計算量は $O(V+C)$ である。

## Random Walk CSP

```cpp
random_walk_csp(
    domain_sizes, constraint_scopes, constraint_violated,
    random, steps_per_restart, restart_limit);
```

違反制約、そのスコープ内の変数、値域内の値を順にランダムに選んで変更する。時間計算量は $O(RSCQ)$、追加空間計算量は $O(V+C)$ である。

## `ConstraintSearchResult`

代入 `assignment`、その代入での違反制約の番号 `violated_constraints`、`steps`、`restarts`、全制約の充足を表す `found` を返す。

## 注意点

- 各値域は空でなく（違反時は `std::invalid_argument`）、スコープ内の変数番号は範囲内でなければならない（違反時は `std::out_of_range`）。`weight_increment` は正の有限値でなければならない。
- スコープが空の制約が違反している場合は、変更によって修復できない。
- コールバックは渡された代入を変更してはならない。
- どちらの手法も不完全であり、失敗は充足不能の証明ではない。
