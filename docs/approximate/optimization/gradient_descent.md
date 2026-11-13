---
title: Gradient Descent (勾配降下法)
documentation_of: ../../../src/approximate/optimization/gradient_descent.hpp
---

## `gradient_descent`

```cpp
auto result = approximate::optimization::gradient_descent(
    initial, objective, gradient, max_iterations,
    learning_rate, tolerance = 1e-8, lower = {}, upper = {}
);
```

`x -= learning_rate * gradient(x)` を繰り返す。`gradient` は `const std::vector<Real>&` を受け取り、同じ次元の `std::vector<Real>` を返す。`lower` と `upper` をともに省略すると制約なし、指定すると各更新の後に箱へ射影する。勾配の無限大ノルムが `tolerance` 以下なら、更新する前に停止する。

`Real` は浮動小数点型、初期点は空でない有限値、`learning_rate` は正の有限値、`tolerance` は 0 以上の有限値でなければならない。箱制約の次元は初期点と一致し、`lower[i] <= upper[i]` かつ初期点が箱の中になければならない。これらに違反すると `std::invalid_argument` を送出する。目的関数・勾配の非有限値には `std::domain_error`、更新のオーバーフローには `std::overflow_error` を送出する。

返り値は `OptimizationResult<Real>` である。完了した更新の回数を $I$ とすると `evaluations == I + 1` であり、`gradient_evaluations` は収束判定で停止した場合は $I+1$、反復上限で停止した場合は $I$ である。

## 計算量

次元を $D$、勾配 1 回のコストを $G$、目的関数 1 回のコストを $E$、完了した更新の回数を $I$ とする。時間計算量は $O(E+I(G+D+E))$、追加空間計算量は $O(D)$ である。収束判定だけを行う最後の勾配呼び出しがある場合は、時間に $O(G+D)$ が加わる。

## 注意点

- 目的関数・勾配は、指定した次元と有限値の条件を満たさなければならない
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
