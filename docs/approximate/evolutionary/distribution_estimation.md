---
title: Distribution Estimation Algorithms (分布推定アルゴリズム)
documentation_of: ../../../src/approximate/evolutionary/distribution_estimation.hpp
---

明示 population の交叉でなく、優良個体から sampling distribution を更新する
EDA 群。All functions minimize by default and use an external URBG.

## `DistributionEstimationResult`

binary `individual`, `score`, final Bernoulli `probabilities`,
`iterations`, `evaluations` を持つ。scalar access は $O(1)$、二つの vector copy は
bit 数 $B$ に対して $O(B)$。

## `estimation_of_distribution_algorithm`

```cpp
estimation_of_distribution_algorithm(
    parameters, sample, update, evaluate,
    generations, population_size, elite_count, rng, compare = {}
)
```

`sample(parameters, rng)` で population を生成し、上位 elite を
`update(elites, parameters)` に渡す問題非依存 EDA。
世代 $G$、population $N$、sampling $S$、評価 $E$、update $U$ の
時間計算量は $O(G(N(S+E)+N\log N+U))$、追加空間 $O(N)$ 個体。

## `univariate_marginal_distribution_algorithm`

```cpp
univariate_marginal_distribution_algorithm<Real>(
    bit_count, evaluate, generations, population_size,
    elite_count, rng, margin = 0, compare = {}
)
```

elite の各 bit 平均を次世代 Bernoulli probability にする UMDA。
probability は `[margin, 1-margin]` に clamp する。
時間計算量 $O(G(N(B+E)+N\log N))$、追加空間 $O(NB)$。

## `population_based_incremental_learning`

```cpp
population_based_incremental_learning<Real>(
    bit_count, evaluate, generations, population_size,
    learning_rate, mutation_probability, mutation_shift,
    rng, compare = {}
)
```

世代最良 bit へ probability を learning rate だけ近づけ、その後各 bit を指定確率で
random bit 方向へ mutation shift する PBIL。
時間計算量 $O(GN(B+E))$、追加空間 $O(B)$。

## 注意点

- bit、世代、population、elite は正で `elite <= population`。
- UMDA margin は有限な $[0,0.5)$。PBIL の3 rate は有限な $[0,1]$ で、
  learning rate のみ正。
- generic EDA の update は次の sampling に有効な parameter を返す責任がある。
- 評価回数は generic EDA/UMDA/PBIL でそれぞれ正確に $GN$。
- Independent marginals cannot model linkage between variables; compact GA,
  UMDA, and PBIL may therefore stagnate on strongly epistatic objectives.
