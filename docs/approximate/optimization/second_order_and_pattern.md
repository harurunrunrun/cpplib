---
title: Newton, Conjugate Gradient, and Pattern Search (Newton・共役勾配・パターン探索)
documentation_of: ../../../src/approximate/optimization/second_order_and_pattern.hpp
---

以下、次元を $N$、反復回数を $I$、目的関数 1 回のコストを $F$、勾配 1 回のコストを $G$、ヘッセ行列 1 回のコストを $H$ とする。

## ニュートン法

```cpp
newton_method(
    initial, objective, gradient, hessian,
    maximum_iterations, maximum_evaluations,
    regularization = 1e-8, initial_step = 1,
    tolerance = 1e-8, armijo = 1e-4,
    line_reduction = 0.5, minimum_step = 1e-12,
    lower = {}, upper = {});
```

`hessian(point)` が返す密なヘッセ行列に対し、対角への正則化と部分ピボット選択付きの消去法でニュートン方向を求め、Armijo 条件のバックトラッキングを行う。降下方向でなければ負の勾配方向に戻す。返り値 `NewtonOptimizationResult<Real>` は、通常の結果 `optimization`（`OptimizationResult<Real>`）とヘッセ行列の評価回数 `hessian_evaluations` を持つ。

直線探索の最大候補数を $A$ とすると、時間計算量は $O(I(H+N^3+G+A(F+N)))$、空間計算量は $O(N^2)$ である。

## 線形共役勾配法

```cpp
conjugate_gradient<Real>(
    matrix_vector, right_hand_side, initial,
    maximum_iterations, tolerance = 1e-10);
```

対称正定値な線形方程式 $Ax=b$ を、行列ベクトル積のコールバック `matrix_vector` だけを使って解く。返り値 `ConjugateGradientResult<Real>` は `point`, `residual_norm`, `iterations`, `converged` を持つ。`matrix_vector` 1 回のコストを $M$ として、時間計算量は $O(I(M+N))$、空間計算量は $O(N)$ である。

## 一般パターン探索

```cpp
pattern_search(
    initial, objective, directions,
    maximum_iterations, maximum_evaluations,
    initial_step = 1, reduction = 0.5,
    step_tolerance = 1e-8, lower = {}, upper = {});
```

各パターン方向の候補を調べて最良の改善点へ移り、改善がなければステップ幅を縮小する。方向の数を $P$ とすると、時間計算量は $O(IP(F+N))$、空間計算量は $O(PN)$ である。

## 注意点

- ニュートン法のヘッセ行列は正方かつ有限で、正則化後も正則でなければならない。非有限な要素や特異な行列には `std::domain_error` を送出する
- 線形共役勾配法は、曲率が正でない反復で `std::domain_error` を送出する
- パターン探索の方向は有限・非零で初期点と同じ次元でなければならない。評価回数の上限は初期点の評価を含む
