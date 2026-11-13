---
title: AdaGrad (適応勾配法)
documentation_of: ../../../src/approximate/optimization/adagrad.hpp
---

## `adagrad`

```cpp
auto result = approximate::optimization::adagrad(
    initial, objective, gradient, max_iterations,
    learning_rate, epsilon = 1e-8, tolerance = 1e-8, lower = {}, upper = {}
);
```

各座標について勾配の二乗和 $s_i$ を保持し、`x[i] -= learning_rate * g[i] / (sqrt(s[i]) + epsilon)` で更新する。`learning_rate` と `epsilon` は正の有限値、`tolerance` は 0 以上の有限値でなければならない。勾配の無限大ノルムが `tolerance` 以下なら、更新する前に停止する。上下限を省略すると制約なし、指定すると各更新の後に箱へ射影する。

初期点・目的関数・勾配・箱制約の要件と例外は `gradient_descent` と同じである。勾配の二乗和がオーバーフローした場合は `std::overflow_error` を送出する。返り値は `OptimizationResult<Real>` である。

## 計算量

次元を $D$、勾配 1 回のコストを $G$、目的関数 1 回のコストを $E$、更新回数を $I$ とすると、時間計算量は $O(E+I(G+D+E))$、追加空間計算量は勾配の二乗和を含めて $O(D)$ である。

## 注意点

- 目的関数・勾配は、各 API で示した次元と有限値の条件を満たさなければならない
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
