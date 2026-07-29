---
title: Advanced SAT Local Search (高度なSAT局所探索)
documentation_of: ../../../src/approximate/constraint/advanced_sat_local_search.hpp
---

CNF は `CnfFormula`、返値は既存の `SatSearchResult` を用いる。すべて
`approximate::constraint` 名前空間にある。

## Novelty / Novelty+

```cpp
novelty(formula, random, steps_per_restart, restart_limit,
        novelty_noise = 0.3);
novelty_plus(formula, random, steps_per_restart, restart_limit,
             novelty_noise = 0.3, random_walk_probability = 0.01);
```

ランダムに選んだ未充足節で、flip 後の充足節数が最大の変数を選ぶ。最大候補が
その節で最も新しく flip された変数なら、`novelty_noise` の確率で第2候補を
選ぶ。Novelty+ はその前にランダム walk を混合する。

節の総 literal 数を `L`、最大節長を `K` とすると、時間計算量は
`O(restart_limit * steps_per_restart * K * L)`、追加空間計算量は
`O(V + K + M)`。

## Random Walk SAT

```cpp
random_walk_sat(formula, random, steps_per_restart, restart_limit);
```

ランダムな未充足節からランダムな変数を選び flip する。時間計算量は
`O(restart_limit * steps_per_restart * L)`、追加空間計算量は `O(V+M)`。

## Breakout / PAWS / SAPS / Clause Weighting

```cpp
breakout_method(formula, random, steps, restarts,
                weight_increment = 1);
paws(formula, random, steps, restarts,
     smoothing_probability = 0.15);
saps(formula, random, steps, restarts, scaling = 1.3,
     smoothing_probability = 0.05, smoothing = 0.8);
clause_weighting_sat(formula, random, steps, restarts,
                     weight_increment = 1,
                     smoothing_probability = 0);
```

weighted satisfied score が最大となる flip を選び、局所最適では未充足節の
weight を更新する。Breakout と Clause Weighting は加算、PAWS は加算と確率的
decrement、SAPS は乗算と weight を1へ戻す smoothing を行う。

各 step で全変数の weighted score を調べるため、時間計算量は
`O(restarts * steps * V * L)`、追加空間計算量は `O(V+M)`。

## 注意点

確率は有限な `[0,1]`、加算量は正有限、SAPS の scaling は1より大きい有限値
でなければならない。これらは不完全な局所探索であり、`found == false` は
UNSAT の証明ではない。固定 seed の URBG を渡せば再現可能である。
