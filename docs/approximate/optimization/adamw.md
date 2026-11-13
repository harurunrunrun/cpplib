---
title: AdamW (重み減衰付き適応モーメント推定)
documentation_of: ../../../src/approximate/optimization/adamw.hpp
---

## `adamw`

```cpp
auto result = approximate::optimization::adamw(
    initial, objective, gradient, max_iterations, learning_rate,
    weight_decay, beta1 = 0.9, beta2 = 0.999, epsilon = 1e-8,
    tolerance = 1e-8, lower = {}, upper = {}
);
```

Adam の更新項とは別に `weight_decay * x[i]` を減じる、分離型の重み減衰（decoupled weight decay）を行う。`weight_decay` は 0 以上の有限値、`beta1` と `beta2` は $[0, 1)$、`learning_rate` と `epsilon` は正の有限値で指定する。勾配の無限大ノルムが `tolerance` 以下なら、重み減衰を適用せずに停止する。

初期点・目的関数・勾配・箱制約の要件と例外は `gradient_descent` と同じである。`beta1`, `beta2` が範囲外なら `std::invalid_argument`、モーメントがオーバーフローした場合は `std::overflow_error` を送出する。返り値は `OptimizationResult<Real>` である。

## 計算量

次元を $D$、勾配 1 回のコストを $G$、目的関数 1 回のコストを $E$、更新回数を $I$ とすると、時間計算量は $O(E+I(G+D+E))$、追加空間計算量は $O(D)$ である。

## 注意点

- 目的関数・勾配は、各 API で示した次元と有限値の条件を満たさなければならない
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
