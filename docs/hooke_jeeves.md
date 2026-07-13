---
title: Hooke–Jeeves法
documentation_of: ../src/approximate/optimization/hooke_jeeves.hpp
---

## `hooke_jeeves`

```cpp
auto result = approximate::optimization::hooke_jeeves(
    initial, objective, max_iterations, max_evaluations,
    initial_step, reduction_factor, pattern_factor, tolerance,
    lower, upper
);
```

各座標について正方向、負方向の順でexploratory moveを行う。改善方向を見つけた座標では
反対側を評価せず、その時点の改善点から次座標を調べる。exploratory move全体が改善したら
`explored + pattern_factor * (explored - base)`も1回調べ、さらに改善するときだけ採用する。
改善がなければstepを`reduction_factor`倍し、`step <= tolerance`で収束する。
乱数を使わず、同値では先に調べた候補を保持する。

目的関数値を点とともにcacheし、現在点と同じ射影候補や、正方向で改善済みの場合の負方向を
評価しない。返却値は`OptimizationResult<Real>`、`gradient_evaluations == 0`、
`evaluations <= max_evaluations`である。評価・反復上限では現在の最良点を未収束で返す。

次元を $D$、反復数を $I$、目的関数1回を $F$ とする。候補は1反復最大`2D+1`個で、
目的関数呼び出しは最大 $1+I(2D+1)$ 回。現在の実装は各座標候補の点をcopyするため、
時間は $O(F+I(D^2+DF))$、追加領域は $O(D)$。結果member参照は $O(1)$、点の列挙は
$O(D)$。

初期点は空でない有限値で、0次元は`std::invalid_argument`。`max_evaluations`、
`initial_step`、`pattern_factor`は正、`reduction_factor`は`(0,1)`、`tolerance`は
0以上で、実数parameterは全て有限でなければならない。箱制約は次元一致・有限・順序付きで
初期点を含む必要がある。不正parameterは`std::invalid_argument`、目的関数の非有限値は
`std::domain_error`、候補計算のoverflowは`std::overflow_error`。stepが丸めで縮小不能なら
`std::runtime_error`を送出する。
