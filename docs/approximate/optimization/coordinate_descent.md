---
title: Coordinate Descent (座標降下法)
documentation_of: ../../../src/approximate/optimization/coordinate_descent.hpp
---

## `coordinate_descent`

```cpp
auto result = approximate::optimization::coordinate_descent(
    initial, objective, coordinate_gradient, max_sweeps,
    learning_rate, tolerance = 1e-8, lower = {}, upper = {}
);
```

座標を 0 から順に巡回する座標勾配降下法（cyclic coordinate gradient descent）である。`coordinate_gradient(point, i)` は現在の点における $i$ 番目の偏微分を返す。偏微分の絶対値が `tolerance` 以下の座標は更新しない。それ以外の座標は `x[i] -= learning_rate * coordinate_gradient(x, i)` で直ちに更新するため、後続の座標の偏微分は更新後の点で評価される。1 周（スイープ）で更新がなければ収束とみなす。箱制約を指定した場合は、各座標の更新を区間へ射影する。

初期点は空でない有限値、`learning_rate` は正の有限値、`tolerance` は 0 以上の有限値でなければならない。返り値は `OptimizationResult<Real>` であり、`iterations` は更新のあったスイープ数、`gradient_evaluations` は偏微分の呼び出し回数を表す。

## 計算量

次元を $D$、偏微分 1 回のコストを $C$、目的関数 1 回のコストを $E$、完了したスイープ数を $S$ とする。時間計算量は $O(E+SD(C+E))$、追加空間計算量は返す点を除いて $O(1)$ である。最後に収束判定だけのスイープがある場合は、時間に $O(DC)$ が加わる。目的関数は、実際に値が変わった座標ごとに 1 回呼ばれる。

## 注意点

- 目的関数または偏微分が非有限値を返すと `std::domain_error`、更新がオーバーフローすると `std::overflow_error` を送出する
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
