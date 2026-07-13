---
title: Island Model and Memetic Genetic Algorithm (島モデルとミーム型遺伝的アルゴリズム)
documentation_of: ../src/approximate/evolutionary/hybrid_genetic_algorithm.hpp
---

`approximate::evolutionary`名前空間で、世代交代GAに局所改善または島間移住を
加えた探索を提供する。目的値、選択、交叉、変異、`Compare`の規約は
`genetic_algorithm.hpp`と同じである。乱数生成器は参照で渡す。

## `memetic_genetic_algorithm`

```cpp
auto result = memetic_genetic_algorithm(
    population, evaluate, select, crossover, mutate, local_improve,
    generations, rng, elite_count, compare
);
```

各子に変異を適用した後、`local_improve(individual, rng)`で破壊的な局所改善を行う。
目的値順の先頭`elite_count`個はそのまま次世代へ残す。初期集団が空、または
elite数が集団サイズを超える場合は`std::invalid_argument`を送出する。
返却型は`EvolutionResult<Individual, Score>`である。

## `island_model_genetic_algorithm`

```cpp
auto result = island_model_genetic_algorithm(
    islands, evaluate, select, crossover, mutate,
    epochs, generations_per_epoch, migrants_per_epoch,
    rng, elite_count, compare
);
```

各島で独立に世代交代GAを実行し、epoch末に各島の上位個体を次の島へ環状に移す。
移住個体は移住先の下位個体を置き換える。移住は全送出個体を確定してから行う。
島数は1以上、各島は非空でなければならない。elite数または1 epochの移住数が
いずれかの島のサイズを超える場合は`std::invalid_argument`を送出する。
返却値は全島で得た最良個体を持つ`EvolutionResult<Individual, Score>`である。
`iterations`は`epochs * generations_per_epoch`を表す。

## 計算量

全島の個体数を$N$、島数を$K$、世代の総数を$G$、1 epochの移住数を$R$、
最大島サイズを$N_{max}$とする。評価・選択・交叉・変異・局所改善1回の時間を
$E,S,C,M,L$、個体1個の領域を$P$とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `memetic_genetic_algorithm` | $O(NE+G(N\log N+N(S+C+M+L+E)))$ | $O(NP+N)$ |
| `island_model_genetic_algorithm` | $O(NE+G(KN_{max}\log N_{max}+N(S+C+M+E))+\mathrm{epochs}\,K(N_{max}\log N_{max}+R))$ | $O(NP+N)$ |
