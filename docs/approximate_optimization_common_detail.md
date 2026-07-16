---
title: Optimization Common Helpers (最適化共通補助)
documentation_of: ../src/approximate/optimization/detail/common.hpp
---

## Optimization Common Helpers (最適化共通補助)

連続最適化で共有する入力検査、評価、射影、反復処理の内部API。

## API

### `validate_real_type<Real>()`, `validate_point(point)`, `validate_box(lower, upper, dimension)`, `validate_problem(point, lower, upper)`

型、初期点、箱型制約を検査する。次元を $D$ として時間計算量は `validate_real_type` が $O(1)$、ほかは $O(D)$、追加空間計算量はいずれも $O(1)$。

### `validate_nonnegative(value, message)`, `validate_positive(value, message)`, `increment_count(count)`

scalar parameterまたはcounterを検査する。時間・追加空間計算量は $O(1)$。

### `project_and_validate(point, lower, upper)`

点の有限性を検査し、境界があれば箱へ射影する。時間計算量は $O(D)$、追加空間計算量は $O(1)$。

### `evaluate_objective(objective, point, evaluations)`

目的関数を一度評価し、回数と有限性を検査する。目的関数の計算量を $T_F,S_F$ として時間計算量は $O(T_F)$、追加空間計算量は $O(S_F)$。

### `infinity_norm(values)`, `validate_gradient(gradient, dimension)`

無限大normを計算する、または勾配を検査する。時間計算量は $O(D)$、追加空間計算量は $O(1)$。

### `FirstOrderUpdate<Real>`

次の点と現在の勾配を保持する。空間計算量は $O(D)$。

### `run_first_order(point, objective, max_iterations, tolerance, lower, upper, step)`

`step(point, iteration)` が返す `FirstOrderUpdate` を反復する。実行反復数を $I$、1回のstepと目的関数の時間計算量を $T_S,T_F$ として時間計算量は $O(I(T_S+T_F+D))$、追加空間計算量は $O(D+S_S+S_F)$。

## 注意点

`approximate::optimization::detail` の内部APIである。`Real` は浮動小数点型で、初期点は非空・有限かつ指定境界内でなければならない。不正入力、非有限な目的値・勾配、counter overflowを例外として拒否する。
