---
title: Gradient Descent (勾配降下法)
documentation_of: ../src/approximate/optimization/gradient_descent.hpp
---

## `gradient_descent`

```cpp
auto result = approximate::optimization::gradient_descent(
    initial, objective, gradient, max_iterations,
    learning_rate, tolerance, lower, upper
);
```

`x -= learning_rate * gradient(x)`を反復する。`gradient`は
`const std::vector<Real>&`を受け取り、同じ次元の`std::vector<Real>`を返す。
`lower`と`upper`をともに省略すると制約なし、指定すると各更新後に箱へ射影する。
勾配の無限大ノルムが`tolerance`以下なら更新前に停止する。

`Real`は浮動小数点型、初期点は空でない有限値、`learning_rate`は正の有限値、
`tolerance`は0以上の有限値でなければならない。箱制約の次元は初期点と一致し、
`lower[i] <= upper[i]`かつ初期点が箱内でなければならない。目的関数・勾配の
非有限値には`std::domain_error`、更新のoverflowには`std::overflow_error`を送出する。

返却値は`OptimizationResult<Real>`である。完了した更新を$I$とすると
`evaluations == I + 1`であり、収束判定で停止した場合の
`gradient_evaluations`は$I+1`、反復上限の場合は$I`である。

## 計算量

次元を$D$、勾配1回を$G$、目的関数1回を$E$、完了した更新を$I$とする。
時間計算量は$O(E+I(G+D+E))$、追加空間計算量は$O(D)$である。収束判定だけを行う
最後の勾配呼び出しがある場合は時間に$O(G+D)$を加える。

## 注意点

目的関数・勾配・座標微分は各APIで示した次元と有限値の条件を満たす必要がある。非凸関数に対する大域最適解や、反復上限内での収束は保証しない。
