---
title: Particle Swarm Optimization (粒子群最適化)
documentation_of: ../../../src/approximate/evolutionary/particle_swarm_optimization.hpp
---

箱型制約付きの連続最小化問題に粒子群最適化を適用する。

## `particle_swarm_optimization`

```cpp
auto result = approximate::evolutionary::particle_swarm_optimization(
    positions, lower, upper, evaluate, iterations,
    inertia, cognitive_coefficient, social_coefficient,
    rng, compare = {}
);
```

各粒子の初速度を 0 とし、慣性項、自己の最良位置への項、全体の最良位置への項で速度を更新する。位置が箱型制約を越えた座標は境界に戻し、その座標の速度を 0 にする。乱数生成器は参照で渡す。`Compare` のデフォルトは `std::less<>` であり、変更すれば最大化もできる。戻り値の型は `EvolutionResult<std::vector<Real>, Score>` である。

## 計算量

粒子数を $N$、次元を $D$、反復回数を $I$、目的関数 1 回の時間を $E$ とすると、時間計算量は $O(NE+IN(D+E))$、追加空間計算量は $O(ND+N)$ である。

## 注意点

- 3 つの係数は非負の有限値でなければならない。次元の不一致、空の個体、不正な上下限、箱の外にある初期位置では `std::invalid_argument` を送出する。
- 目的関数と遺伝的操作は呼び出し中に有効でなければならず、比較関数は一貫した順序を与えなければならない。
- 結果の再現には、同じ乱数生成器の状態・入力・標準ライブラリの実装が必要である。大域最適性は保証しない。
