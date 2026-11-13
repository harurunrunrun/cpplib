---
title: Optimization Common Helpers (最適化共通補助)
documentation_of: ../../../../src/approximate/optimization/detail/common.hpp
---

連続最適化で共有する、入力の検査・評価・射影・反復処理の内部 API である。以下、次元を $D$ とする。

## API

### `validate_real_type<Real>()`, `validate_point(point)`, `validate_box(lower, upper, dimension)`, `validate_problem(point, lower, upper)`

型、初期点、箱制約を検査する。時間計算量は `validate_real_type` が $O(1)$、ほかは $O(D)$、追加空間計算量はいずれも $O(1)$ である。

### `validate_nonnegative(value, message)`, `validate_positive(value, message)`, `increment_count(count)`

スカラーのパラメータを検査する、またはカウンタを増やす。時間計算量・追加空間計算量は $O(1)$ である。

### `project_and_validate(point, lower, upper)`

点の有限性を検査し、境界があれば箱へ射影する。時間計算量は $O(D)$、追加空間計算量は $O(1)$ である。

### `evaluate_objective(objective, point, evaluations)`

目的関数を 1 回評価し、評価回数を数え、有限性を検査する。目的関数の時間・空間を $T_F, S_F$ として、時間計算量は $O(T_F)$、追加空間計算量は $O(S_F)$ である。

### `infinity_norm(values)`, `validate_gradient(gradient, dimension)`

無限大ノルムを計算する、または勾配を検査する。時間計算量は $O(D)$、追加空間計算量は $O(1)$ である。

### `FirstOrderUpdate<Real>`

次の点と現在の勾配を保持する。空間計算量は $O(D)$ である。

### `run_first_order(point, objective, max_iterations, tolerance, lower, upper, step)`

`step(point, iteration)` が返す `FirstOrderUpdate` を繰り返し適用する。実行した反復回数を $I$、1 回の `step` と目的関数の時間を $T_S, T_F$、空間を $S_S, S_F$ として、時間計算量は $O(I(T_S+T_F+D))$、追加空間計算量は $O(D+S_S+S_F)$ である。

## 注意点

- `approximate::optimization::detail` の内部 API である
- `Real` は浮動小数点型で、初期点は空でなく、有限かつ指定した境界内になければならない
- 不正な入力には `std::invalid_argument`、非有限な目的値・勾配には `std::domain_error`、カウンタや座標のオーバーフローには `std::overflow_error` を送出する
