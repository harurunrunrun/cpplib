---
title: Powell's Direction Set Method (Powell方向集合法)
documentation_of: ../src/approximate/optimization/powell.hpp
---

## `powell`

```cpp
auto result = approximate::optimization::powell(
    initial, objective, max_iterations, max_evaluations,
    initial_step, line_reduction, line_tolerance, tolerance,
    lower, upper
);
```

単位座標方向から開始する勾配不要のPowell方向集合法。各方向では正方向、負方向の順に
候補を比較する決定的pattern line searchを行い、改善がないとstepを
`line_reduction`倍する。1 sweepの変位を正規化して、最大減少を得た方向と入れ替える。
同値の候補は先に調べた正方向を保持する。

初期点の目的関数値と受理した候補値を保持し、同じ点を不必要に再評価しない。箱への射影で
現在点と同じになった候補も評価しない。返却値は`OptimizationResult<Real>`で、
`gradient_evaluations == 0`。`evaluations <= max_evaluations`であり、評価上限では現在までの
最良点を未収束として返す。sweepの変位がなくなるか、点と値の変化が`tolerance`以下なら
収束する。

次元を $D$、sweep数を $I$、目的関数1回を $F$、構築したline-search候補総数を $Q$、
実際の目的関数評価総数を $L$ とする。時間は $O(D^2+ID^2+QD+LF)$、追加領域は
方向集合を含め $O(D^2)$。各結果member参照は $O(1)$、結果点の列挙は $O(D)$。

初期点は空でない有限値で、0次元は`std::invalid_argument`。`max_evaluations`、
`initial_step`、`line_tolerance`は正で、step類は有限かつ
`line_tolerance <= initial_step`、`line_reduction`は`(0,1)`、`tolerance`は0以上の
有限値でなければならない。箱制約は次元一致・有限・順序付きで初期点を含む必要がある。

目的関数のNaN/無限大は`std::domain_error`、候補・変位のoverflowは
`std::overflow_error`。浮動小数点の丸めにより`tolerance`より大きいstepをこれ以上縮小
できない場合はline search失敗として`std::runtime_error`を送出する。
