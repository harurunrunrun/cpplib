---
title: RMSProp (RMSProp最適化)
documentation_of: ../src/approximate/optimization/rmsprop.hpp
---

## `rmsprop`

```cpp
auto result = approximate::optimization::rmsprop(
    initial, objective, gradient, max_iterations,
    learning_rate, decay, epsilon, tolerance, lower, upper
);
```

勾配二乗の指数移動平均$s_i$を
`s[i] = decay * s[i] + (1 - decay) * g[i] * g[i]`で更新し、
`x[i] -= learning_rate * g[i] / (sqrt(s[i]) + epsilon)`を行う。
`decay`は`[0, 1)`、`learning_rate`と`epsilon`は正の有限値で指定する。
勾配の無限大ノルムが`tolerance`以下なら更新前に停止する。

初期点・目的関数・勾配・箱制約の要件と例外は`gradient_descent`と同じである。
二乗平均がoverflowした場合は`std::overflow_error`を送出する。返却値は
`OptimizationResult<Real>`である。

## 計算量

次元を$D$、勾配1回を$G$、目的関数1回を$E$、更新回数を$I$とすると、
時間計算量は$O(E+I(G+D+E))$、追加領域は$O(D)$である。
