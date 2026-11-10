---
title: Genetic Algorithm (遺伝的アルゴリズム)
documentation_of: ../../../src/approximate/evolutionary/genetic_algorithm.hpp
---

`approximate::evolutionary` 名前空間で、任意の個体型に対する最小化の遺伝的アルゴリズムを提供する。`Compare` のデフォルトは `std::less<>` であり、`std::greater<>` などに変更すれば最大化できる。

次の呼び出し可能オブジェクトを渡す。

- `evaluate(individual)`: 個体の目的値を返す。
- `select(population, scores, rng)`: 親として使う個体の添字を返す。範囲外の添字を返すと `std::out_of_range` を送出する。
- `crossover(lhs, rhs, rng)`: 2 個体から新しい個体を作って返す。
- `mutate(individual, rng)`: 個体を破壊的に変異させる。

乱数生成器は参照で渡す。同じ初期状態の乱数生成器と決定的なコールバックを使えば、結果も同じになる。

## `generational_genetic_algorithm`

```cpp
auto result = generational_genetic_algorithm(
    population, evaluate, select, crossover, mutate,
    generations, rng, elite_count = 1, compare = {}
);
```

目的値の順で先頭の `elite_count` 個をそのまま残し、残りを選択・交叉・変異で作り直す。戻り値の型は `EvolutionResult<Individual, Score>` である。

## `steady_state_genetic_algorithm`

```cpp
auto result = steady_state_genetic_algorithm(
    population, evaluate, select, crossover, mutate,
    offspring_count, rng, compare = {}
);
```

子を 1 個ずつ生成し、現在の最悪の個体より良ければその個体と置き換える。戻り値の型は `EvolutionResult<Individual, Score>` である。

## 計算量

集団サイズを $N$、世代数を $G$、生成する子の総数を $L$、評価・選択・交叉・変異 1 回の時間をそれぞれ $E$, $S$, $C$, $M$、個体 1 個の領域を $P$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `generational_genetic_algorithm` | $O(NE+G(N\log N+N(S+C+M+E)))$ | $O(NP+N)$ |
| `steady_state_genetic_algorithm` | $O(NE+L(S+C+M+E+N))$ | $O(NP+N)$ |

戻り値の構築には、個体と目的値のコピーのコストが加わる。

## 注意点

- 初期集団が空の場合、および `elite_count` が集団サイズを超える場合は `std::invalid_argument` を送出する。
- 目的関数と遺伝的操作は呼び出し中に有効でなければならず、比較関数は一貫した順序を与えなければならない。
- 結果の再現には、同じ乱数生成器の状態・入力・標準ライブラリの実装が必要である。大域最適性は保証しない。
