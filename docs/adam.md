---
title: Adam
documentation_of: ../src/approximate/optimization/adam.hpp
---

## `adam`

```cpp
auto result = approximate::optimization::adam(
    initial, objective, gradient, max_iterations, learning_rate,
    beta1, beta2, epsilon, tolerance, lower, upper
);
```

勾配の一次・二次モーメントの指数移動平均を保持し、反復番号に応じた
bias correctionを施して更新する。`beta1`と`beta2`は`[0, 1)`、
`learning_rate`と`epsilon`は正の有限値で指定する。勾配の無限大ノルムが
`tolerance`以下なら更新前に停止する。上下限を指定すると更新後に箱へ射影する。

初期点・目的関数・勾配・箱制約の要件と例外は`gradient_descent`と同じである。
モーメントがoverflowした場合は`std::overflow_error`を送出する。返却値は
`OptimizationResult<Real>`である。

## 計算量

次元を$D$、勾配1回を$G$、目的関数1回を$E$、更新回数を$I$とすると、
時間計算量は$O(E+I(G+D+E))$、追加領域は一次・二次モーメントを含め$O(D)$である。
