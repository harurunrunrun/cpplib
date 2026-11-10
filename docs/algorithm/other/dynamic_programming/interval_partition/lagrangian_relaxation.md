---
title: Lagrangian Relaxation (Lagrange緩和)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/lagrangian_relaxation.hpp
---

選択数の制約を整数のペナルティに置き換え（Alien DP）、ペナルティに対して選択数が単調になる部分問題を二分探索して、ちょうど指定個数を選ぶときの最適値を復元する。

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

`solve(penalty)` は「元の目的値 $+ \mathrm{penalty} \times \mathrm{count}$」を最小化した状態を返す。最小値が同じ場合は小さい `count` を返す規約とする。
`count <= target_count` となる最小のペナルティ $\lambda$ を二分探索で求め、`solve(λ).penalized_cost - λ * target_count` を返す。

## 時間計算量

ペナルティの探索区間の幅を $R$、`solve` の時間・空間を $T, S$ として $O(T\log R)$ 時間である。

## 空間計算量

$O(S)$。

## 注意点

- `Cost` は整数型でなければならない（`static_assert` で検査する）。
- 最適な選択数がペナルティの増加に対して非増加でなければならない。探索区間が必要な境界を含むこと、および離散凸性により補正した値が指定個数での最適値になることを前提とする。
- `target_count < 0` または `maximum_penalty < minimum_penalty` では `std::runtime_error` を送出する。
