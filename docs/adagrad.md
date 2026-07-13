---
title: AdaGrad
documentation_of: ../src/approximate/optimization/adagrad.hpp
---

## `adagrad`

```cpp
auto result = approximate::optimization::adagrad(
    initial, objective, gradient, max_iterations,
    learning_rate, epsilon, tolerance, lower, upper
);
```

各座標について勾配二乗和$s_i$を保持し、
`x[i] -= learning_rate * g[i] / (sqrt(s[i]) + epsilon)`で更新する。
`learning_rate`と`epsilon`は正の有限値、`tolerance`は0以上の有限値で
なければならない。勾配の無限大ノルムが`tolerance`以下なら更新前に停止する。
上下限を省略すると制約なし、指定すると各更新後に箱へ射影する。

初期点・目的関数・勾配・箱制約の要件と例外は`gradient_descent`と同じである。
勾配二乗和がoverflowした場合は`std::overflow_error`を送出する。返却値は
`OptimizationResult<Real>`である。

## 計算量

次元を$D$、勾配1回を$G$、目的関数1回を$E$、更新回数を$I$とすると、
時間計算量は$O(E+I(G+D+E))$、追加領域は勾配二乗和を含め$O(D)$である。
