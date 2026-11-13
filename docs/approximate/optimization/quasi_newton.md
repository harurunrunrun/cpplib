---
title: BFGS / L-BFGS (準ニュートン法)
documentation_of: ../../../src/approximate/optimization/quasi_newton.hpp
---

滑らかな目的関数を、勾配と Armijo 条件のバックトラッキング直線探索で最小化する準ニュートン法である。返り値は `OptimizationResult<Real>` である。

以下、次元を $D$、受理した更新の回数を $I$、目的関数 1 回のコストを $F$、勾配 1 回のコストを $G$、直線探索での目的関数の評価の総数を $L$ とする。

## `bfgs`

```cpp
auto result = approximate::optimization::bfgs(
    initial, objective, gradient, max_iterations, max_evaluations,
    initial_step = 1, tolerance = 1e-8, armijo = 1e-4,
    line_reduction = 0.5, minimum_step = 1e-12, lower = {}, upper = {}
);
```

逆ヘッセ行列の近似を単位行列から始め、曲率条件を満たす更新対で BFGS 更新を行う。`gradient` は点を受け取り、同じ次元の `vector<Real>` を返す。勾配の無限大ノルムが `tolerance` 以下なら収束とみなす。探索方向が降下方向でなくなった場合は、逆ヘッセ行列の近似を単位行列に戻す。

各反復の目的関数値は、直線探索が返した値を再利用する。初期値を 1 回評価した後、`evaluations` は Armijo 条件の候補を実際に評価した回数だけ増え、`max_evaluations` を超えない。`gradient_evaluations` は初期勾配と、受理した更新の後の勾配の回数である。

時間計算量は $O(D^2+I(D^2+G)+L(D+F))$、追加空間計算量は $O(D^2)$ である。結果のメンバの参照は $O(1)$、結果の点の列挙は $O(D)$ である。

## `lbfgs`

```cpp
auto result = approximate::optimization::lbfgs(
    initial, objective, gradient, max_iterations, max_evaluations,
    history_size = 10, initial_step = 1, tolerance = 1e-8, armijo = 1e-4,
    line_reduction = 0.5, minimum_step = 1e-12, lower = {}, upper = {}
);
```

直近 `history_size` 個の曲率対だけを保持し、two-loop recursion で探索方向を求める L-BFGS である。十分に正の曲率を持たない対は保存せず、降下方向でなくなれば履歴を破棄する。乱数は使わず、同じ入力の目的関数・勾配に対して決定的に動作する。

履歴の上限を $M$ とすると、時間計算量は $O(D+I(MD+G)+L(D+F))$、追加空間計算量は $O(MD)$ である。目的関数・勾配の評価回数の数え方は `bfgs` と同じである。

## 注意点

- `Real` は浮動小数点型で、初期点は空でない有限値でなければならない（0 次元は `std::invalid_argument`）
- `max_evaluations` と `history_size` は正、`initial_step` と `minimum_step` は正の有限値で `minimum_step <= initial_step`、`armijo` と `line_reduction` は $(0,1)$、`tolerance` は 0 以上の有限値でなければならない
- `max_iterations == 0` では初期点の目的関数だけを評価し、未収束で返す
- 箱制約を使う場合は両端の次元が一致し、有限で `lower[i] <= upper[i]` を満たし、初期点が箱の中になければならない。候補は箱へ射影する。準ニュートン方向を射影すると降下移動がなくなる場合は、ヘッセ行列の近似または履歴をリセットして射影した最急降下方向で再試行し、その方向にも実行可能な降下移動がない場合に限り収束とする
- 評価回数・反復回数の上限に達した場合は `converged == false` で通常終了する。実際に評価した候補が `minimum_step` まで Armijo 条件を満たさない場合は `std::runtime_error` を送出する
- 不正なパラメータ・勾配の次元には `std::invalid_argument`、目的関数または勾配の NaN・無限大には `std::domain_error`、点・内積・行列の更新のオーバーフローには `std::overflow_error` を送出する
