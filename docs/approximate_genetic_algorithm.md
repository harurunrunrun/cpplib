---
title: Genetic Algorithm (遺伝的アルゴリズム)
documentation_of: ../src/approximate/evolutionary/genetic_algorithm.hpp
---

`approximate::evolutionary`名前空間で、任意の個体型に対する最小化遺伝的
アルゴリズムを提供する。`Compare`の既定値は`std::less<>`であり、
`std::greater<>`などに変更すれば最大化できる。

次のcallableを渡す。

- `evaluate(individual)`: 個体の目的値を返す。
- `select(population, scores, rng)`: 親として使う添字を返す。範囲外の添字では
  `std::out_of_range`を送出する。
- `crossover(lhs, rhs, rng)`: 2個体から新しい個体を返す。
- `mutate(individual, rng)`: 個体を破壊的に変異させる。

乱数生成器は参照で渡される。同じ初期状態の乱数生成器と決定的なcallableを
使えば結果も同じになる。初期集団が空の場合は`std::invalid_argument`を送出する。

## `generational_genetic_algorithm`

```cpp
auto result = generational_genetic_algorithm(
    population, evaluate, select, crossover, mutate,
    generations, rng, elite_count, compare
);
```

目的値順の先頭`elite_count`個を保存し、残りを選択・交叉・変異で作り直す。
`elite_count`の既定値は1である。集団サイズを超える値では
`std::invalid_argument`を送出する。結果型は`EvolutionResult<Individual, Score>`。

## `steady_state_genetic_algorithm`

```cpp
auto result = steady_state_genetic_algorithm(
    population, evaluate, select, crossover, mutate,
    offspring_count, rng, compare
);
```

子を1個ずつ生成し、現在の最悪個体より良い場合にその個体を置き換える。
結果型は`EvolutionResult<Individual, Score>`。

## 計算量

$N$を集団サイズ、$G$を世代数、$L$を生成する子の総数、評価・選択・
交叉・変異1回の時間をそれぞれ$E,S,C,M$、個体1個の領域を$P$とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `generational_genetic_algorithm` | $O(NE+G(N\log N+N(S+C+M+E)))$ | $O(NP+N)$ |
| `steady_state_genetic_algorithm` | $O(NE+L(S+C+M+E+N))$ | $O(NP+N)$ |

返却値の構築には個体と目的値のコピー費用が加わる。
