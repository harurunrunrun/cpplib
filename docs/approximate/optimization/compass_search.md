---
title: Compass Search (コンパス探索)
documentation_of: ../../../src/approximate/optimization/compass_search.hpp
---

## `compass_search`

```cpp
auto result = approximate::optimization::compass_search(
    initial, objective, max_iterations, initial_step,
    reduction_factor = 0.5, tolerance = 1e-8, lower = {}, upper = {}
);
```

現在点から各座標の正負の方向へ `step` だけ動かした候補をすべて評価し、最良の改善候補へ移動する。改善がなければ `step *= reduction_factor` とする。`step <= tolerance` で収束とみなす。乱数は使わず、目的値が等しい候補では列挙順を保つ。箱制約を指定した場合は候補を箱へ射影し、現在点と同じになった候補は評価しない。

`initial_step` は正の有限値、`reduction_factor` は $(0, 1)$、`tolerance` は 0 以上の有限値でなければならない（違反は `std::invalid_argument`）。初期点・目的関数・箱制約の要件は `gradient_descent` と同じである。非有限な目的関数値には `std::domain_error`、座標計算のオーバーフローには `std::overflow_error` を送出する。返り値は `OptimizationResult<Real>` であり、`gradient_evaluations` は常に 0 である。

## 計算量

次元を $D$、目的関数 1 回のコストを $E$、反復回数を $I$ とする。目的関数の呼び出しは最大 $1+2DI$ 回、時間計算量は $O(E+ID(D+E))$、追加空間計算量は $O(D)$ である。

## 注意点

- 目的関数は、指定した次元と有限値の条件を満たさなければならない
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
