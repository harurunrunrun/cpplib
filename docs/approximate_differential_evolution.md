---
title: Differential Evolution (差分進化法)
documentation_of: ../src/approximate/evolutionary/differential_evolution.hpp
---

箱制約付き連続最小化にDE/rand/1/binを適用する。

## `differential_evolution`

```cpp
auto result = approximate::evolutionary::differential_evolution(
    population, lower, upper, evaluate, generations,
    differential_weight, crossover_probability, rng, compare
);
```

個体は`std::vector<Real>`で、4個体以上必要である。各対象個体と異なる3個体を
選び、`a + differential_weight * (b - c)`をbinomial crossoverして候補を作る。
少なくとも1座標は必ず変異ベクトルから取る。候補は上下限へclampし、対象より
良いときだけ次世代へ残す。世代更新は同期的に行う。

`differential_weight`は0以上の有限値、`crossover_probability`は`[0, 1]`で
なければならない。次元不一致、空の個体、箱外の初期個体でも
`std::invalid_argument`を送出する。乱数生成器は参照で渡す。
`Compare`の既定値は`std::less<>`であり、変更すれば最大化できる。
返却型は`EvolutionResult<std::vector<Real>, Score>`である。

## 計算量

集団サイズを$N$、次元を$D$、世代数を$G$、目的関数1回の時間を$E$とすると、
時間計算量は$O(NE+GN(D+E))$、追加領域は$O(ND+N)$である。
