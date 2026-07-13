---
title: Momentum法
documentation_of: ../src/approximate/optimization/momentum.hpp
---

## `momentum_descent`

```cpp
auto result = approximate::optimization::momentum_descent(
    initial, objective, gradient, max_iterations,
    learning_rate, momentum, tolerance, lower, upper
);
```

`v = momentum * v + gradient(x)`、`x -= learning_rate * v`を反復する。
`momentum`は`[0, 1)`で指定する。勾配の無限大ノルムが`tolerance`以下なら
更新前に停止する。上下限を省略すると制約なし、指定すると各更新後に箱へ射影する。

初期点・目的関数・勾配・箱制約の要件と例外は`gradient_descent`と同じである。
`learning_rate`は正の有限値、`tolerance`は0以上の有限値でなければならない。
返却値は`OptimizationResult<Real>`である。

## 計算量

次元を$D$、勾配1回を$G$、目的関数1回を$E$、更新回数を$I$とすると、
時間計算量は$O(E+I(G+D+E))$、追加領域は速度を含め$O(D)$である。
