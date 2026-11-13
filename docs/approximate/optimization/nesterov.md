---
title: Nesterov Accelerated Gradient (Nesterov加速勾配法)
documentation_of: ../../../src/approximate/optimization/nesterov.hpp
---

## `nesterov_accelerated_gradient`

```cpp
auto result = approximate::optimization::nesterov_accelerated_gradient(
    initial, objective, gradient, max_iterations,
    learning_rate, momentum, tolerance = 1e-8, lower = {}, upper = {}
);
```

先読み点 `x - learning_rate * momentum * v` で勾配を評価し、`v = momentum * v + gradient(lookahead)`、`x -= learning_rate * v` と更新する。箱制約を指定した場合は、先読み点と更新後の点を箱へ射影する。`momentum` は $[0, 1)$、`learning_rate` は正の有限値で指定する。先読み点での勾配の無限大ノルムが `tolerance` 以下なら停止する。

初期点・目的関数・勾配・箱制約の要件と例外は `gradient_descent` と同じである。返り値は `OptimizationResult<Real>` である。

## 計算量

次元を $D$、勾配 1 回のコストを $G$、目的関数 1 回のコストを $E$、更新回数を $I$ とすると、時間計算量は $O(E+I(G+D+E))$、追加空間計算量は $O(D)$ である。

## 注意点

- 目的関数・勾配は、指定した次元と有限値の条件を満たさなければならない
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
