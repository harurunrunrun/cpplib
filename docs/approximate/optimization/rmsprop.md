---
title: RMSProp (RMSProp最適化)
documentation_of: ../../../src/approximate/optimization/rmsprop.hpp
---

## `rmsprop`

```cpp
auto result = approximate::optimization::rmsprop(
    initial, objective, gradient, max_iterations,
    learning_rate, decay = 0.9, epsilon = 1e-8, tolerance = 1e-8,
    lower = {}, upper = {}
);
```

勾配の二乗の指数移動平均 $s_i$ を `s[i] = decay * s[i] + (1 - decay) * g[i] * g[i]` で更新し、`x[i] -= learning_rate * g[i] / (sqrt(s[i]) + epsilon)` とする。`decay` は $[0, 1)$、`learning_rate` と `epsilon` は正の有限値で指定する。勾配の無限大ノルムが `tolerance` 以下なら、更新する前に停止する。

初期点・目的関数・勾配・箱制約の要件と例外は `gradient_descent` と同じである。二乗平均がオーバーフローした場合は `std::overflow_error` を送出する。返り値は `OptimizationResult<Real>` である。

## 計算量

次元を $D$、勾配 1 回のコストを $G$、目的関数 1 回のコストを $E$、更新回数を $I$ とすると、時間計算量は $O(E+I(G+D+E))$、追加空間計算量は $O(D)$ である。

## 注意点

- 目的関数・勾配は、指定した次元と有限値の条件を満たさなければならない
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
