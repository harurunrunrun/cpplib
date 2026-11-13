---
title: Stochastic Descent Methods (確率的降下法)
documentation_of: ../../../src/approximate/optimization/stochastic_descent.hpp
---

以下、次元を $N$、反復回数を $I$、目的関数 1 回のコストを $F$ とする。返り値はいずれも `OptimizationResult<Real>` である。

## 確率的勾配降下法

```cpp
stochastic_gradient_descent(
    initial, objective, sample_gradient, sample_count, random,
    maximum_iterations, learning_rate, tolerance = 0,
    lower = {}, upper = {});
```

各反復で $[0, \texttt{sample_count})$ から一様ランダムに標本の添字を選び、`sample_gradient(point, sample)` で更新する。標本勾配 1 回のコストを $G$ として、時間計算量は $O(I(F+G+N))$、空間計算量は $O(N)$ である。

## ミニバッチ勾配降下法

```cpp
mini_batch_gradient_descent(
    initial, objective, sample_gradient, sample_count, batch_size,
    random, maximum_iterations, learning_rate, tolerance = 0,
    lower = {}, upper = {});
```

復元抽出した `batch_size` 個の標本勾配を平均して更新する。$B = \texttt{batch_size}$ として、時間計算量は $O(I(F+B(G+N)))$、空間計算量は $O(N)$ である。

## 確率的座標降下法

```cpp
stochastic_coordinate_descent(
    initial, objective, coordinate_gradient, random,
    maximum_iterations, learning_rate, tolerance = 0,
    lower = {}, upper = {});
```

一様ランダムに座標を選び、その偏微分だけで直ちに更新する。偏微分のコールバック 1 回のコストを $C$ として、時間計算量は $O(I(F+C))$、空間計算量は $O(N)$ である。

## 注意点

- `Real` は浮動小数点型で、初期点・目的値・勾配・学習率は有限でなければならない
- 標本数と `batch_size` は正でなければならない
- 箱制約を渡すと、各更新の後に射影する
- `gradient_evaluations` は、標本勾配または偏微分のコールバックを実際に呼んだ回数である
- 確率的勾配に対する `tolerance` の判定は、選ばれた推定値だけに基づく
