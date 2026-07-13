---
title: Nelder–Mead法
documentation_of: ../src/approximate/optimization/nelder_mead.hpp
---

## `nelder_mead`

```cpp
auto result = approximate::optimization::nelder_mead(
    initial, objective, max_iterations, initial_step, tolerance,
    reflection, expansion, contraction, shrink, lower, upper
);
```

初期点と各座標を`initial_step`だけ動かした点から$D+1$点のsimplexを作り、
反射・膨張・収縮・縮小を反復する。箱制約を指定した場合は全候補を箱へ射影する。
上側へ動かせない初期頂点は下側を試す。simplexの座標直径と目的関数値の幅が
ともに`tolerance`以下なら収束とする。同値の頂点順は安定に保たれる。
`reflection`は重心から最悪点の反対方向への倍率、`expansion`は重心から
反射点への倍率である。

`initial_step`と`reflection`は正の有限値、`expansion`は1より大きい有限値、
`contraction`と`shrink`は`(0, 1)`、`tolerance`は0以上の有限値でなければ
ならない。初期点は空でない有限値で箱内に置く。目的関数の非有限値には
`std::domain_error`、候補計算のoverflowには`std::overflow_error`を送出する。
返却値は`OptimizationResult<Real>`であり、`gradient_evaluations`は常に0である。

`max_iterations == 0`でも初期simplexの$D+1$点は評価し、その最良点を返す。
箱の幅が0の座標では頂点が重複し得る。

## 計算量

次元を$D$、目的関数1回を$E$、反復回数を$I$とする。目的関数の呼び出しは
最大$D+1+(D+2)I$回である。時間計算量は
$O((D+1)E+I(D^2+DE))$、simplexを含む追加領域は$O(D^2)$である。
