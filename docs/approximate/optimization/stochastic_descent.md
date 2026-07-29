---
title: Stochastic Descent Methods (確率的降下法)
documentation_of: ../../../src/approximate/optimization/stochastic_descent.hpp
---

## Stochastic Gradient Descent

```cpp
stochastic_gradient_descent(
    initial, objective, sample_gradient, sample_count, random,
    maximum_iterations, learning_rate, tolerance = 0,
    lower = {}, upper = {});
```

各反復で一様ランダムな sample index を選び、
`sample_gradient(point, sample)` で更新する。時間計算量は
`O(I(F+G+N))`、空間計算量は `O(N)`。`F` は目的関数、`G` はsample勾配、
`N` は次元の計算量。

## Mini-Batch Gradient Descent

```cpp
mini_batch_gradient_descent(
    initial, objective, sample_gradient, sample_count, batch_size,
    random, maximum_iterations, learning_rate, tolerance = 0,
    lower = {}, upper = {});
```

復元抽出した `batch_size` 個の sample 勾配を平均する。時間計算量は
`O(I(F+B(G+N)))`、空間計算量は `O(N)`。

## Stochastic Coordinate Descent

```cpp
stochastic_coordinate_descent(
    initial, objective, coordinate_gradient, random,
    maximum_iterations, learning_rate, tolerance = 0,
    lower = {}, upper = {});
```

一様ランダムな座標を選び、その偏微分だけで直ちに更新する。時間計算量は
`O(I(F+C))`、空間計算量は `O(N)`。`C` は偏微分 callback の計算量。

## 注意点

`Real` は浮動小数点型、初期点・目的値・勾配・学習率は有限でなければならない。
sample 数と batch size は正である。box bounds を渡すと各更新後に射影する。
`gradient_evaluations` はsample勾配または偏微分 callback の実呼出し回数である。
確率的勾配に対する `tolerance` 判定は選ばれた推定値だけに基づく。
