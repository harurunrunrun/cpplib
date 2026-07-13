---
title: Advanced Optimization Helpers (高度最適化補助)
documentation_of: ../src/approximate/optimization/detail/advanced_common.hpp
---

# Advanced Optimization Helpers (高度最適化補助)

勾配法・直線探索で共有する評価回数管理とArmijo探索の内部API。

## API

### `validate_evaluation_limit(max_evaluations)`, `has_evaluation_budget(evaluations, max_evaluations, required=1)`

評価上限を検査し、残りbudgetを判定する。時間・追加空間計算量は $O(1)$。

### `evaluate_gradient(gradient, point, gradient_evaluations)`

勾配を一度評価し、回数と次元・有限性を検査する。次元を $D$、勾配計算量を $T_G,S_G$ として時間計算量は $O(T_G+D)$、追加空間計算量は $O(S_G+D)$。

### `checked_dot(left, right)`, `negative_gradient(gradient)`

内積または負勾配をoverflow検査付きで計算する。時間計算量は $O(D)$、追加空間計算量はそれぞれ $O(1)$、$O(D)$。

### `checked_real<Real>(value, message)`

`long double` を有限な `Real` へ変換する。時間・追加空間計算量は $O(1)$。

### `validate_backtracking_parameters(initial_step, armijo, reduction, minimum_step)`

Armijo backtrackingのparameter範囲を検査する。時間・追加空間計算量は $O(1)$。

### `LineSearchStatus`, `LineSearchResult<Real>`

直線探索の終了理由と、候補点・目的値を保持する返値型。点の保持空間は $O(D)$。

### `armijo_backtracking(...)`

箱型制約へ射影しながらArmijo条件を満たすstepを探索する。試行回数を $L$、目的関数の計算量を $T_F,S_F$ として時間計算量は $O(L(D+T_F))$、追加空間計算量は $O(D+S_F)$。

## 注意点

`approximate::optimization::detail` の内部APIである。次元不一致、非有限値、overflow、不正parameterを例外として拒否する。目的関数・勾配関数の例外は伝播する。
