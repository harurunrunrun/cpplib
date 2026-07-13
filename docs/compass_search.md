---
title: Compass Search (コンパス探索)
documentation_of: ../src/approximate/optimization/compass_search.hpp
---

## `compass_search`

```cpp
auto result = approximate::optimization::compass_search(
    initial, objective, max_iterations, initial_step,
    reduction_factor, tolerance, lower, upper
);
```

現在点から各座標の正負方向へ`step`だけ動かした候補をすべて評価し、最良の
改善候補へ移動する。改善がなければ`step *= reduction_factor`とする。
`step <= tolerance`で収束とする。乱数を使わず、同値候補では入力順を保つ。
箱制約を指定した場合は候補を箱へ射影し、現在点と同じ候補は評価しない。

`initial_step`は正の有限値、`reduction_factor`は`(0, 1)`、`tolerance`は
0以上の有限値でなければならない。初期点・目的関数・箱制約の要件は
`gradient_descent`と同じである。非有限な目的関数値には`std::domain_error`、
座標計算のoverflowには`std::overflow_error`を送出する。返却値は
`OptimizationResult<Real>`であり、`gradient_evaluations`は常に0である。

## 計算量

次元を$D$、目的関数1回を$E$、反復回数を$I$とする。目的関数の呼び出しは
最大$1+2DI$回、時間計算量は$O(E+ID(D+E))$、追加領域は$O(D)$である。
