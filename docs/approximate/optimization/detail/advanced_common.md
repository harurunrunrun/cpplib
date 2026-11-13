---
title: Advanced Optimization Helpers (高度最適化補助)
documentation_of: ../../../../src/approximate/optimization/detail/advanced_common.hpp
---

勾配法・直線探索で共有する、評価回数の管理と Armijo 条件による直線探索の内部 API である。以下、次元を $D$ とする。

## API

### `validate_evaluation_limit(max_evaluations)`, `has_evaluation_budget(evaluations, max_evaluations, required = 1)`

評価回数の上限を検査し、残りの評価回数で `required` 回評価できるかを判定する。時間計算量・追加空間計算量は $O(1)$ である。

### `evaluate_gradient(gradient, point, gradient_evaluations)`

勾配を 1 回評価し、評価回数を数え、次元と有限性を検査する。勾配計算の時間・空間を $T_G, S_G$ として、時間計算量は $O(T_G+D)$、追加空間計算量は $O(S_G+D)$ である。

### `checked_dot(left, right)`, `negative_gradient(gradient)`

内積、または負の勾配をオーバーフロー検査付きで計算する。時間計算量は $O(D)$、追加空間計算量はそれぞれ $O(1)$, $O(D)$ である。

### `checked_real<Real>(value, message)`

`long double` の値を有限な `Real` に変換する。時間計算量・追加空間計算量は $O(1)$ である。

### `validate_backtracking_parameters(initial_step, armijo, reduction, minimum_step)`

Armijo 条件によるバックトラッキングのパラメータの範囲を検査する。時間計算量・追加空間計算量は $O(1)$ である。

### `LineSearchStatus`, `LineSearchResult<Real>`

直線探索の終了理由と、候補点・目的値を保持する返り値型である。点の保持に $O(D)$ の空間を使う。

### `armijo_backtracking(point, value, gradient, direction, objective, evaluations, max_evaluations, initial_step, armijo, reduction, minimum_step, lower, upper)`

箱制約へ射影しながら、Armijo 条件を満たすステップ幅を探索する。試行回数を $L$、目的関数の時間・空間を $T_F, S_F$ として、時間計算量は $O(L(D+T_F))$、追加空間計算量は $O(D+S_F)$ である。

## 注意点

- `approximate::optimization::detail` の内部 API である
- 次元の不一致や不正なパラメータには `std::invalid_argument`、オーバーフローには `std::overflow_error`、ステップ幅をそれ以上縮小できない場合や、評価上限内に Armijo 条件を満たせなかった場合は `std::runtime_error` を送出する
- 目的関数・勾配関数が送出した例外はそのまま伝播する
