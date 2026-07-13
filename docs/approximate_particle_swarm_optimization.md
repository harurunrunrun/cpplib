---
title: Particle Swarm Optimization
documentation_of: ../src/approximate/evolutionary/particle_swarm_optimization.hpp
---

箱制約付き連続最小化に粒子群最適化を適用する。

## `particle_swarm_optimization`

```cpp
auto result = approximate::evolutionary::particle_swarm_optimization(
    positions, lower, upper, evaluate, iterations,
    inertia, cognitive_coefficient, social_coefficient,
    rng, compare
);
```

各粒子の初速度を0とし、慣性項、自己最良への項、全体最良への項で速度を更新する。
位置が箱制約を越えた座標は境界へ戻し、その座標の速度を0にする。乱数生成器は
参照で渡す。3係数は0以上の有限値でなければならない。次元不一致、空の個体、
不正な上下限、箱外の初期位置では`std::invalid_argument`を送出する。
`Compare`の既定値は`std::less<>`であり、変更すれば最大化できる。
返却型は`EvolutionResult<std::vector<Real>, Score>`である。

## 計算量

粒子数を$N$、次元を$D$、反復数を$I$、目的関数1回の時間を$E$とすると、
時間計算量は$O(NE+IN(D+E))$、追加領域は$O(ND+N)$である。
