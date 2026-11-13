---
title: SPSA (同時摂動確率近似)
documentation_of: ../../../src/approximate/optimization/spsa.hpp
---

## `spsa`

```cpp
auto result = approximate::optimization::spsa(
    initial, objective, urbg, max_iterations, max_evaluations,
    learning_rate, perturbation, learning_exponent = 0.602,
    perturbation_exponent = 0.101, stability = 10, tolerance = 1e-8,
    lower = {}, upper = {}
);
```

同時摂動確率近似法（Simultaneous Perturbation Stochastic Approximation）である。反復番号を $k=1,2,\ldots$ とし、`learning_rate`, `perturbation`, `learning_exponent`, `perturbation_exponent`, `stability` をそれぞれ $a, c, \alpha, \gamma, A$ として、

$$
a_k=\frac{a}{(A+k)^{\alpha}},\qquad
c_k=\frac{c}{k^{\gamma}}
$$

を使う。`urbg` から各座標につき 1 個の Rademacher 符号 $\delta$ を生成し、$x+c_k\delta$ と $x-c_k\delta$ での目的関数値の差から全座標の勾配を同時に推定して、`x -= a_k * estimate` と更新する。`urbg` は左辺値参照で受け取るため、呼び出し後の乱数状態は呼び出し側に残る。

各反復を始める前に、目的関数の評価を最大 3 回分確保する。通常は正負の摂動点と更新後の点の 3 回で、初期点を含めて最大 `1 + 3 * iterations` 回評価する。射影により摂動点・更新後の点が現在点や互いと同じになった場合はキャッシュした値を再利用するので、実際の `evaluations` はこれより少なくなる。残りの評価回数が 3 回未満なら、乱数を消費せずに未収束で終了する。

返り値は `OptimizationResult<Real>` である。1 回の同時摂動による勾配推定を 1 回と数え、`gradient_evaluations == iterations` である。更新量の無限大ノルムが `tolerance` 以下なら収束とみなす。評価回数・反復回数の上限では未収束のまま通常終了する。

## 計算量

次元を $D$、反復回数を $I$、目的関数 1 回のコストを $F$、乱数生成器を 1 回進めるコストを $R$ とすると、時間計算量は $O(F+I(D(1+R)+F))$、目的関数の呼び出しは最大 $1+3I$ 回、追加空間計算量は $O(D)$ である。結果のメンバの参照は $O(1)$、結果の点の列挙は $O(D)$ である。

## 注意点

- 初期点は空でない有限値でなければならない（0 次元は `std::invalid_argument`）
- `max_evaluations`, `learning_rate`, `perturbation`, 2 つの指数は正、`stability` と `tolerance` は 0 以上で、実数のパラメータはすべて有限でなければならない。箱制約は次元が一致し、有限で、`lower[i] <= upper[i]` を満たし、初期点を含まなければならない。違反は `std::invalid_argument` を送出する
- 目的関数の NaN・無限大には `std::domain_error`、ゲイン・摂動・目的関数値の差・勾配の推定・更新のオーバーフローには `std::overflow_error` を送出する
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
