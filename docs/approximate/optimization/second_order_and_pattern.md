---
title: Newton, Conjugate Gradient, and Pattern Search (Newton・共役勾配・パターン探索)
documentation_of: ../../../src/approximate/optimization/second_order_and_pattern.hpp
---

## Newton Method

```cpp
newton_method(
    initial, objective, gradient, hessian,
    maximum_iterations, maximum_evaluations,
    regularization = 1e-8, initial_step = 1,
    tolerance = 1e-8, armijo = 1e-4,
    line_reduction = 0.5, minimum_step = 1e-12,
    lower = {}, upper = {});
```

`hessian(point)` の dense Hessian に対し、対角 regularization と部分 pivoting
付き消去で Newton 方向を求め、Armijo backtracking を行う。降下方向でなければ
負勾配へ戻す。`NewtonOptimizationResult` は通常の `optimization` 結果と
`hessian_evaluations` を持つ。

次元を `N`、line search の最大候補数を `A` とすると、時間計算量は
`O(I(H+N^3+G+A(F+N)))`、空間計算量は `O(N^2)`。

## Linear Conjugate Gradient

```cpp
conjugate_gradient<Real>(
    matrix_vector, right_hand_side, initial,
    maximum_iterations, tolerance = 1e-10);
```

対称正定値線形系 `A*x=b` を matrix-vector callback のみで解く。
時間計算量は `O(I(M+N))`、空間計算量は `O(N)`。`M` は
`matrix_vector` の計算量。

## General Pattern Search

```cpp
pattern_search(
    initial, objective, directions,
    maximum_iterations, maximum_evaluations,
    initial_step = 1, reduction = 0.5,
    step_tolerance = 1e-8, lower = {}, upper = {});
```

各 pattern direction の候補を調べて最良改善点へ移り、改善がなければ step を
縮小する。方向数を `P` とすると時間計算量は `O(I*P*(F+N))`、
空間計算量は `O(PN)`。

## 注意点

Newton の Hessian は正方・有限で、regularization 後も非特異でなければならない。
linear CG は正曲率でない反復を `std::domain_error` とする。Pattern Search の
方向は有限・非零・初期点と同次元、評価上限は初期評価を含む。
