---
title: 非線形共役勾配法（Polak–Ribiere+）
documentation_of: ../src/approximate/optimization/nonlinear_conjugate_gradient.hpp
---

## `nonlinear_conjugate_gradient`

```cpp
auto result = approximate::optimization::nonlinear_conjugate_gradient(
    initial, objective, gradient, max_iterations, max_evaluations,
    initial_step, tolerance, armijo, line_reduction, minimum_step,
    lower, upper
);
```

非線形共役勾配法で滑らかな目的関数を最小化する。係数は

$$
\beta_k=\max\left(0,
 \frac{g_{k+1}^{\mathsf T}(g_{k+1}-g_k)}{g_k^{\mathsf T}g_k}\right)
$$

というPolak–Ribiere+を使う。方向が降下方向でなくなれば最急降下方向へrestartする。
Armijo backtrackingで受理した目的関数値をそのまま次反復へ渡し、同じ点を目的関数で
再評価しない。乱数を使わず、同じ入力には決定的に動作する。

返却値は`OptimizationResult<Real>`。`evaluations`は初期点1回とline searchで実際に
評価した候補数で`max_evaluations`以下、`gradient_evaluations`は初期勾配と受理更新後の
勾配数である。勾配の無限大normが`tolerance`以下、または射影後に実行可能な降下移動が
ないとき`converged == true`となる。共役方向の射影で降下移動がなくなった場合は最急降下
方向へrestartして再試行し、その射影方向でも動けない場合だけ収束する。評価・反復上限では
未収束で通常終了する。

次元を $D$、受理更新数を $I$、目的関数1回を $F$、勾配1回を $G$、line searchの評価総数を
$L$ とすると、時間は $O(D+I(D+G)+L(D+F))$、追加領域は $O(D)$。
結果member参照は $O(1)$、結果点の列挙は $O(D)$。

初期点は空でない有限値であり、0次元は`std::invalid_argument`。
`max_evaluations`は正、`initial_step`・`minimum_step`は正の有限値、
`minimum_step <= initial_step`、`armijo`・`line_reduction`は`(0,1)`、`tolerance`は
0以上の有限値でなければならない。箱制約の条件は`bfgs`と同じ。
目的関数・勾配の非有限値、次元不一致、数値overflow、line search失敗の例外も
`bfgs`と同じである。
