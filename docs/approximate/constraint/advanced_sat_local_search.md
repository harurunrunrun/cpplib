---
title: Advanced SAT Local Search (高度なSAT局所探索)
documentation_of: ../../../src/approximate/constraint/advanced_sat_local_search.hpp
---

CNF は `CnfFormula`、戻り値は既存の `SatSearchResult` を用いる。全て `approximate::constraint` 名前空間にある。以下、変数数を $V$、節数を $M$、全リテラル数を $L$、最大の節長を $K$ とする。また、再始動回数（`restart_limit` / `restarts`）を $R$、1 回の再始動あたりのステップ数（`steps_per_restart` / `steps`）を $S$ とする。

## Novelty / Novelty+

```cpp
novelty(formula, random, steps_per_restart, restart_limit,
        novelty_noise = 0.3);
novelty_plus(formula, random, steps_per_restart, restart_limit,
             novelty_noise = 0.3, random_walk_probability = 0.01);
```

ランダムに選んだ未充足節の中で、反転（flip）後の充足節数が最大になる変数を選ぶ。最良の候補がその節で最も最近反転された変数なら、確率 `novelty_noise` で 2 番目の候補を選ぶ。Novelty+ はその前に、確率 `random_walk_probability` でランダムウォーク（節内の変数を一様に選ぶ）を混ぜる。

時間計算量は $O(RSKL)$、追加空間計算量は $O(V+K+M)$ である。

## Random Walk SAT

```cpp
random_walk_sat(formula, random, steps_per_restart, restart_limit);
```

ランダムな未充足節からランダムに変数を選んで反転する。時間計算量は $O(RSL)$、追加空間計算量は $O(V+M)$ である。

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

充足節の重みの和が最大になる反転を選び、局所最適に達したら未充足節の重みを更新する。Breakout と Clause Weighting は加算、PAWS は加算と確率的な減算、SAPS は乗算と、重みを 1 に近づける平滑化を行う。

各ステップで全変数について重み付きスコアを計算し直すため、時間計算量は $O(RSVL)$、追加空間計算量は $O(V+M)$ である。

## 注意点

- 確率は $[0,1]$ の有限値、加算量は正の有限値、SAPS の `scaling` は 1 より大きい有限値でなければならず、満たさない場合は `std::invalid_argument` を送出する。
- 重みやスコアが `long double` の範囲を超える場合は `std::overflow_error` を送出する。
- いずれも不完全な局所探索であり、`found == false` は充足不能（UNSAT）の証明ではない。
- シードを固定した乱数生成器を渡せば、結果は再現できる。
