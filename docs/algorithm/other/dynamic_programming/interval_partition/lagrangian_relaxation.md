---
title: Lagrangian Relaxation (Lagrange緩和)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/lagrangian_relaxation.hpp
---

選択数制約へ整数ペナルティを加え、ペナルティに対して選択数が単調な部分問題を二分探索して、指定個数の最適値を復元する。

## API

```cpp
template<class Cost>
struct LagrangianRelaxationState {
    Cost penalized_cost;
    long long count;
};
```

```cpp
template<class Cost, class Solver>
Cost lagrangian_relaxation_exact_count(
    long long target_count,
    Cost minimum_penalty,
    Cost maximum_penalty,
    Solver solve
);
```

`solve(penalty)` は「元の目的値 + penalty * count」を最小化した状態を返す。同値なら小さい `count` を返す規約とする。

## 時間計算量

ペナルティ区間幅を $R$、`solve` の時間・空間を $T,S$ として $O(T\log R)$ 時間、$O(S)$ 空間。

## 注意点

`Cost` は整数型を想定し、最適選択数がペナルティ増加に対して非増加でなければならない。探索区間が必要な境界を含むこと、離散凸性により補正値が指定個数の最適値となることを前提とする。不正な範囲では `runtime_error`。
