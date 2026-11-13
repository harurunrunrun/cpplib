---
title: Nelder–Mead Method (Nelder–Mead法)
documentation_of: ../../../src/approximate/optimization/nelder_mead.hpp
---

## `nelder_mead`

```cpp
auto result = approximate::optimization::nelder_mead(
    initial, objective, max_iterations, initial_step, tolerance = 1e-8,
    reflection = 1, expansion = 2, contraction = 0.5, shrink = 0.5,
    lower = {}, upper = {}
);
```

初期点と、各座標を `initial_step` だけ動かした点から $D+1$ 点の単体（simplex）を作り、反射・拡大・収縮・縮小を繰り返す。箱制約を指定した場合は全候補を箱へ射影する。上側へ動かせない初期頂点は下側を試す。単体の座標方向の直径と目的関数値の幅がともに `tolerance` 以下なら収束とみなす。目的値が等しい頂点の順序は安定に保たれる。`reflection` は重心から最悪点と反対方向への倍率、`expansion` は重心から反射点への倍率である。

`initial_step` と `reflection` は正の有限値、`expansion` は 1 より大きい有限値、`contraction` と `shrink` は $(0, 1)$、`tolerance` は 0 以上の有限値でなければならない（違反は `std::invalid_argument`）。初期点は空でない有限値で、箱の中に置く。返り値は `OptimizationResult<Real>` であり、`gradient_evaluations` は常に 0 である。

`max_iterations == 0` でも初期単体の $D+1$ 点は評価し、その最良点を返す。

## 計算量

次元を $D$、目的関数 1 回のコストを $E$、反復回数を $I$ とする。目的関数の呼び出しは最大 $D+1+(D+2)I$ 回である。時間計算量は $O((D+1)E+I(D^2+DE))$、単体を含む追加空間計算量は $O(D^2)$ である。

## 注意点

- 目的関数の非有限値には `std::domain_error`、候補の計算のオーバーフローには `std::overflow_error` を送出する
- 箱の幅が 0 の座標では頂点が重複し得る
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
