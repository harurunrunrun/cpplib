---
title: Global Continuous Optimization (大域的連続最適化)
documentation_of: ../../../src/approximate/optimization/global_optimization.hpp
---

すべて最小化 API で、`GlobalOptimizationResult<Real>` は `point`、`value`、
`iterations`、`evaluations` を返す。

## Bayesian Optimization

```cpp
bayesian_optimization(
    objective, lower, upper, random,
    initial_samples, iterations, candidates_per_iteration,
    acquisition = BayesianAcquisition::expected_improvement,
    acquisition_parameter = 0.01,
    length_scale = 1, observation_noise = 1e-8);

expected_improvement_optimization(...);
probability_of_improvement_optimization(...);
upper_confidence_bound_optimization(...);
```

RBF kernel の Gaussian process を Cholesky 分解で更新し、一様生成した候補から
acquisition 最大点を評価する。UCB wrapper は最小化のため
`-mean + exploration * deviation` を最大化する。

観測数を `T`、1反復の候補数を `C`、次元を `N` とすると、時間計算量は
`O(I*T^3 + I*C*(T^2+N))`、空間計算量は `O(T^2+TN)`。

## Acquisition Utilities

```cpp
expected_improvement(mean, deviation, best, exploration = 0);
probability_of_improvement(mean, deviation, best, exploration = 0);
upper_confidence_bound(mean, deviation, exploration);
```

前2つは最小化に対する improvement、最後は通常の `mean + kappa*sigma` を返す。
各時間・空間計算量は `O(1)`。

## DIRECT

```cpp
direct_optimization(
    objective, lower, upper,
    maximum_iterations, maximum_evaluations,
    exploration_weight = 1e-4);
```

中心値と box 半径の lower score で分割対象を選び、最長軸を3分割する
deterministic DIRECT search。box 数を `B` とすると時間計算量は
`O(I*(B*N+N*F))`、空間計算量は `O(BN)`。

## Basin Hopping

```cpp
basin_hopping(
    initial, objective, random,
    basin_iterations, local_iterations, maximum_evaluations,
    perturbation, temperature,
    local_step = 1, local_reduction = 0.5,
    lower = {}, upper = {});
```

Gaussian perturbation 後に座標 pattern local search を行い、悪化解を
Metropolis 確率で受理する。時間計算量は
`O(B*L*N*(F+N))`、空間計算量は `O(N)`。

## 注意点

Bayesian/DIRECT は正幅の有限 box bounds を必須とする。Gaussian process は
正の noise と length scale が必要である。Bayesian candidate は有限個の
一様乱数点なので acquisition の連続大域最大化を保証しない。DIRECT と
Basin Hopping も有限 budget の不完全探索である。
