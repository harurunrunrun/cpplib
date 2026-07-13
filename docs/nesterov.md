---
title: Nesterov Accelerated Gradient (Nesterov加速勾配法)
documentation_of: ../src/approximate/optimization/nesterov.hpp
---

## `nesterov_accelerated_gradient`

```cpp
auto result = approximate::optimization::nesterov_accelerated_gradient(
    initial, objective, gradient, max_iterations,
    learning_rate, momentum, tolerance, lower, upper
);
```

`x - learning_rate * momentum * v`で勾配を評価し、
`v = momentum * v + gradient(lookahead)`、`x -= learning_rate * v`を行う。
箱制約を指定した場合はlookaheadと更新後の点を箱へ射影する。
`momentum`は`[0, 1)`、`learning_rate`は正の有限値で指定する。
lookaheadにおける勾配の無限大ノルムが`tolerance`以下なら停止する。

初期点・目的関数・勾配・箱制約の要件と例外は`gradient_descent`と同じである。
返却値は`OptimizationResult<Real>`である。

## 計算量

次元を$D$、勾配1回を$G$、目的関数1回を$E$、更新回数を$I$とすると、
時間計算量は$O(E+I(G+D+E))$、追加領域は$O(D)$である。
