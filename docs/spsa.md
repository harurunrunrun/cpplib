---
title: SPSA
documentation_of: ../src/approximate/optimization/spsa.hpp
---

## `spsa`

```cpp
auto result = approximate::optimization::spsa(
    initial, objective, urbg, max_iterations, max_evaluations,
    learning_rate, perturbation, learning_exponent,
    perturbation_exponent, stability, tolerance, lower, upper
);
```

Simultaneous Perturbation Stochastic Approximation。反復番号を $k=1,2,\ldots$ として

$$
a_k=\frac{\mathtt{learning\_rate}}
          {(\mathtt{stability}+k)^{\mathtt{learning\_exponent}}},\qquad
c_k=\frac{\mathtt{perturbation}}
          {k^{\mathtt{perturbation\_exponent}}}
$$

を使う。`urbg`から各座標につき1個のRademacher符号を生成し、`x+c_k delta`と
`x-c_k delta`の目的関数差から全座標の勾配を同時推定して`x -= a_k estimate`と更新する。
`urbg`は左辺値参照で受け取るため、呼出し後の乱数状態が呼出し側に残る。

各反復を開始する前に最大3回分の目的関数budgetを確保する。通常は正負摂動と更新点の3回、
初期点を含め最大`1+3*iterations`回評価する。射影で摂動点・更新点が現在点または互いに同じ
場合はcache済みの値を再利用し、実際の`evaluations`はこれより少なくなる。
budgetが3回未満なら乱数を消費せず未収束で終了する。

返却値は`OptimizationResult<Real>`。1個の同時摂動勾配推定を1回として
`gradient_evaluations == iterations`である。更新の無限大normが`tolerance`以下なら収束。
評価・反復上限は未収束で通常終了する。

次元を $D$、反復数を $I$、目的関数1回を $F$、URBGを1回進める時間を $R$ とすると、
時間は $O(F+I(D(1+R)+F))$、目的関数呼出しは最大 $1+3I$、追加領域は $O(D)$。
結果member参照は $O(1)$、結果点の列挙は $O(D)$。

初期点は空でない有限値で、0次元は`std::invalid_argument`。`max_evaluations`、
`learning_rate`、`perturbation`、2つのexponentは正、`stability`と`tolerance`は0以上で、
全実数parameterは有限でなければならない。箱制約は次元一致・有限・順序付きで初期点を
含む必要がある。不正parameterは`std::invalid_argument`、目的関数のNaN/無限大は
`std::domain_error`、gain・摂動・目的関数差・勾配推定・更新のoverflowは
`std::overflow_error`を送出する。
