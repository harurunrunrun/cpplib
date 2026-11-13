---
title: Adam (適応モーメント推定)
documentation_of: ../../../src/approximate/optimization/adam.hpp
---

## `adam`

```cpp
auto result = approximate::optimization::adam(
    initial, objective, gradient, max_iterations, learning_rate,
    beta1 = 0.9, beta2 = 0.999, epsilon = 1e-8, tolerance = 1e-8,
    lower = {}, upper = {}
);
```

勾配の 1 次・2 次モーメントの指数移動平均を保持し、反復番号に応じたバイアス補正を施して更新する。`beta1` と `beta2` は $[0, 1)$、`learning_rate` と `epsilon` は正の有限値で指定する。勾配の無限大ノルムが `tolerance` 以下なら、更新する前に停止する。上下限を指定すると、更新の後に箱へ射影する。

初期点・目的関数・勾配・箱制約の要件と例外は `gradient_descent` と同じである。`beta1`, `beta2` が範囲外なら `std::invalid_argument`、モーメントがオーバーフローした場合は `std::overflow_error` を送出する。返り値は `OptimizationResult<Real>` である。

## 計算量

次元を $D$、勾配 1 回のコストを $G$、目的関数 1 回のコストを $E$、更新回数を $I$ とすると、時間計算量は $O(E+I(G+D+E))$、追加空間計算量は 1 次・2 次モーメントを含めて $O(D)$ である。

## 注意点

- 目的関数・勾配は、各 API で示した次元と有限値の条件を満たさなければならない
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
