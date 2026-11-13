---
title: Momentum Method (モーメンタム法)
documentation_of: ../../../src/approximate/optimization/momentum.hpp
---

## `momentum_descent`

```cpp
auto result = approximate::optimization::momentum_descent(
    initial, objective, gradient, max_iterations,
    learning_rate, momentum, tolerance = 1e-8, lower = {}, upper = {}
);
```

`v = momentum * v + gradient(x)`、`x -= learning_rate * v` を繰り返す。`momentum` は $[0, 1)$ で指定する。勾配の無限大ノルムが `tolerance` 以下なら、更新する前に停止する。上下限を省略すると制約なし、指定すると各更新の後に箱へ射影する。

初期点・目的関数・勾配・箱制約の要件と例外は `gradient_descent` と同じである。`learning_rate` は正の有限値、`tolerance` は 0 以上の有限値でなければならない。返り値は `OptimizationResult<Real>` である。

## 計算量

次元を $D$、勾配 1 回のコストを $G$、目的関数 1 回のコストを $E$、更新回数を $I$ とすると、時間計算量は $O(E+I(G+D+E))$、追加空間計算量は速度ベクトルを含めて $O(D)$ である。

## 注意点

- 目的関数・勾配は、指定した次元と有限値の条件を満たさなければならない
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
