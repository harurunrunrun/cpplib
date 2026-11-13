---
title: Nonlinear Conjugate Gradient (Polak–Ribiere+) (非線形共役勾配法)
documentation_of: ../../../src/approximate/optimization/nonlinear_conjugate_gradient.hpp
---

## `nonlinear_conjugate_gradient`

```cpp
auto result = approximate::optimization::nonlinear_conjugate_gradient(
    initial, objective, gradient, max_iterations, max_evaluations,
    initial_step = 1, tolerance = 1e-8, armijo = 1e-4,
    line_reduction = 0.5, minimum_step = 1e-12, lower = {}, upper = {}
);
```

非線形共役勾配法で滑らかな目的関数を最小化する。係数には次の Polak–Ribière+ 公式を使う。

$$
\beta_k=\max\left(0,
 \frac{g_{k+1}^{\mathsf T}(g_{k+1}-g_k)}{g_k^{\mathsf T}g_k}\right)
$$

方向が降下方向でなくなれば、最急降下方向に戻して再開（restart）する。Armijo 条件のバックトラッキングで受理した目的関数値をそのまま次の反復に渡し、同じ点を再評価しない。乱数は使わず、同じ入力には決定的に動作する。

返り値は `OptimizationResult<Real>` である。`evaluations` は初期点の 1 回と直線探索で実際に評価した候補の数で、`max_evaluations` 以下である。`gradient_evaluations` は初期勾配と、受理した更新の後の勾配の数である。勾配の無限大ノルムが `tolerance` 以下になるか、射影後に実行可能な降下移動がないとき `converged == true` となる。共役方向の射影で降下移動がなくなった場合は最急降下方向に戻して再試行し、その射影方向でも動けない場合に限り収束とする。評価回数・反復回数の上限では未収束のまま通常終了する。

## 計算量

次元を $D$、受理した更新の回数を $I$、目的関数 1 回のコストを $F$、勾配 1 回のコストを $G$、直線探索の評価回数の総和を $L$ とすると、時間計算量は $O(D+I(D+G)+L(D+F))$、追加空間計算量は $O(D)$ である。結果のメンバの参照は $O(1)$、結果の点の列挙は $O(D)$ である。

## 注意点

- 初期点は空でない有限値でなければならない（0 次元は `std::invalid_argument`）
- `max_evaluations` は正、`initial_step`・`minimum_step` は正の有限値で `minimum_step <= initial_step`、`armijo`・`line_reduction` は $(0,1)$、`tolerance` は 0 以上の有限値でなければならない。箱制約の条件は `bfgs` と同じである
- 目的関数・勾配の非有限値、次元の不一致、数値のオーバーフロー、直線探索の失敗に対する例外も `bfgs` と同じである
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
