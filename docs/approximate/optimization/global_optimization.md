---
title: Global Continuous Optimization (大域的連続最適化)
documentation_of: ../../../src/approximate/optimization/global_optimization.hpp
---

すべて最小化の API である。返り値 `GlobalOptimizationResult<Real>` は `point`, `value`, `iterations`, `evaluations` を持つ。以下、次元を $N$、反復回数を $I$、目的関数 1 回のコストを $F$ とする。

## ベイズ最適化

```cpp
bayesian_optimization(
    objective, lower, upper, random,
    initial_samples, iterations, candidates_per_iteration,
    acquisition = BayesianAcquisition::expected_improvement,
    acquisition_parameter = 0.01,
    length_scale = 1, observation_noise = 1e-8);

expected_improvement_optimization(
    objective, lower, upper, random,
    initial_samples, iterations, candidates_per_iteration,
    exploration = 0.01, length_scale = 1, observation_noise = 1e-8);
probability_of_improvement_optimization(...);   // 引数は上と同じ
upper_confidence_bound_optimization(...);       // 引数は上と同じ。exploration のデフォルトは 2
```

RBF カーネルのガウス過程を Cholesky 分解で更新し、一様に生成した候補の中から獲得関数が最大の点を評価する。3 つのラッパーは `acquisition` を固定して `bayesian_optimization` を呼ぶ。UCB のラッパーは最小化のため `-mean + exploration * deviation` を最大化する。

観測数を $T$、1 反復の候補数を $C$ とすると、時間計算量は $O(IT^3 + IC(T^2+N))$、空間計算量は $O(T^2+TN)$ である。

## 獲得関数

```cpp
expected_improvement(mean, deviation, best, exploration = 0);
probability_of_improvement(mean, deviation, best, exploration = 0);
upper_confidence_bound(mean, deviation, exploration);
```

前の 2 つは最小化に対する改善量（の期待値・確率）を返し、最後は通常の $\mu + \kappa\sigma$（`mean + exploration * deviation`）を返す。各関数の時間計算量・空間計算量は $O(1)$ である。

## DIRECT

```cpp
direct_optimization(
    objective, lower, upper,
    maximum_iterations, maximum_evaluations,
    exploration_weight = 1e-4);
```

中心での目的値と箱の半径から作る下界スコアで分割対象を選び、最長の軸を 3 分割する決定的な DIRECT 探索である。箱の数を $B$ とすると、時間計算量は $O(I(BN+NF))$、空間計算量は $O(BN)$ である。

## ベイスンホッピング

```cpp
basin_hopping(
    initial, objective, random,
    basin_iterations, local_iterations, maximum_evaluations,
    perturbation, temperature,
    local_step = 1, local_reduction = 0.5,
    lower = {}, upper = {});
```

ガウス分布による摂動の後に座標方向のパターン局所探索を行い、悪化した解を Metropolis 確率で受理する。ベイスンの反復回数を $B$、局所探索の反復回数を $L$ とすると、時間計算量は $O(BLN(F+N))$、空間計算量は $O(N)$ である。

## 注意点

- ベイズ最適化と DIRECT は、幅が正で有限な箱制約 `lower`, `upper` を必須とする
- ベイズ最適化は `initial_samples` と `candidates_per_iteration` が正、`acquisition_parameter` が有限な非負値、`length_scale` と `observation_noise` が正の有限値でなければならない。DIRECT は `maximum_evaluations` が正、`exploration_weight` が有限な非負値でなければならない。違反は `std::invalid_argument` を送出する
- ベイズ最適化の候補は有限個の一様乱数点なので、獲得関数の連続的な大域最大化は保証しない。DIRECT とベイスンホッピングも、有限の評価回数による不完全な探索である
