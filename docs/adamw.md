---
title: AdamW
documentation_of: ../src/approximate/optimization/adamw.hpp
---

## `adamw`

```cpp
auto result = approximate::optimization::adamw(
    initial, objective, gradient, max_iterations, learning_rate,
    weight_decay, beta1, beta2, epsilon, tolerance, lower, upper
);
```

Adamの更新項とは別に`weight_decay * x[i]`を減じるdecoupled weight decayを
行う。`weight_decay`は0以上の有限値、`beta1`と`beta2`は`[0, 1)`、
`learning_rate`と`epsilon`は正の有限値で指定する。勾配の無限大ノルムが
`tolerance`以下ならweight decayを適用せず停止する。

初期点・目的関数・勾配・箱制約の要件と例外は`gradient_descent`と同じである。
モーメントがoverflowした場合は`std::overflow_error`を送出する。返却値は
`OptimizationResult<Real>`である。

## 計算量

次元を$D$、勾配1回を$G$、目的関数1回を$E$、更新回数を$I$とすると、
時間計算量は$O(E+I(G+D+E))$、追加領域は$O(D)$である。
