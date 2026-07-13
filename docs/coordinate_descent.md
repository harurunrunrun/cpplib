---
title: Coordinate Descent (座標降下法)
documentation_of: ../src/approximate/optimization/coordinate_descent.hpp
---

## `coordinate_descent`

```cpp
auto result = approximate::optimization::coordinate_descent(
    initial, objective, coordinate_gradient, max_sweeps,
    learning_rate, tolerance, lower, upper
);
```

座標を0から順に走査するcyclic coordinate gradient descentである。
`coordinate_gradient(point, i)`は現在の点における$i$番目の偏微分を返す。
絶対値が`tolerance`以下の座標は更新しない。それ以外は
`x[i] -= learning_rate * coordinate_gradient(x, i)`で直ちに更新するため、
後続座標の偏微分は更新後の点で評価される。1 sweepで更新がなければ収束とする。
箱制約を指定した場合は各座標更新を区間へ射影する。

初期点は空でない有限値、`learning_rate`は正の有限値、`tolerance`は0以上の
有限値でなければならない。目的関数または偏微分が非有限値を返すと
`std::domain_error`、更新がoverflowすると`std::overflow_error`を送出する。
返却値は`OptimizationResult<Real>`であり、`iterations`は更新のあったsweep数、
`gradient_evaluations`は偏微分の呼び出し回数を表す。

## 計算量

次元を$D$、偏微分1回を$C$、目的関数1回を$E$、完了したsweep数を$S$とする。
時間計算量は$O(E+SD(C+E))$、追加空間計算量は返却する点を除いて$O(1)$である。
最後の収束判定sweepがある場合は時間に$O(DC)$を加える。目的関数は実際に
値が変わった座標ごとに1回呼ばれる。

## 注意点

目的関数・勾配・座標微分は各APIで示した次元と有限値の条件を満たす必要がある。非凸関数に対する大域最適解や、反復上限内での収束は保証しない。
