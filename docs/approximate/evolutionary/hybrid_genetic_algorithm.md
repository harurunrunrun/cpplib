---
title: Island Model and Memetic Genetic Algorithm (島モデルとミーム型遺伝的アルゴリズム)
documentation_of: ../../../src/approximate/evolutionary/hybrid_genetic_algorithm.hpp
---

`approximate::evolutionary` 名前空間で、世代交代型の GA に局所改善または島間の移住を加えた探索を提供する。目的値・選択・交叉・変異・`Compare` の規約は `genetic_algorithm.hpp` と同じである。乱数生成器は参照で渡す。

## `memetic_genetic_algorithm`

```cpp
auto result = memetic_genetic_algorithm(
    population, evaluate, select, crossover, mutate, local_improve,
    generations, rng, elite_count = 1, compare = {}
);
```

各子に変異を適用した後、`local_improve(individual, rng)` で破壊的に局所改善を行う。目的値の順で先頭の `elite_count` 個はそのまま次世代へ残す。戻り値の型は `EvolutionResult<Individual, Score>` である。

## `island_model_genetic_algorithm`

```cpp
auto result = island_model_genetic_algorithm(
    islands, evaluate, select, crossover, mutate,
    epochs, generations_per_epoch, migrants_per_epoch,
    rng, elite_count = 1, compare = {}
);
```

各島で独立に世代交代型の GA を実行し、各エポックの終わりに各島の上位の個体を次の島へ環状に移す。移住した個体は、移住先の下位の個体と置き換わる。移住は、全ての島から送り出す個体を確定してから行う。戻り値は、全ての島で得た最良の個体を持つ `EvolutionResult<Individual, Score>` であり、`iterations` は `epochs * generations_per_epoch` を表す。

## 計算量

全ての島の個体数の合計を $N$、島の数を $K$、世代の総数を $G$、エポック数を $T$、1 エポックあたりの移住数を $R$、最大の島のサイズを $N_{\max}$ とする。評価・選択・交叉・変異・局所改善 1 回の時間を $E$, $S$, $C$, $M$, $L$、個体 1 個の領域を $P$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `memetic_genetic_algorithm` | $O(NE+G(N\log N+N(S+C+M+L+E)))$ | $O(NP+N)$ |
| `island_model_genetic_algorithm` | $O(NE+G(KN_{\max}\log N_{\max}+N(S+C+M+E))+TK(N_{\max}\log N_{\max}+R))$ | $O(NP+N)$ |

## 注意点

- `memetic_genetic_algorithm` で初期集団が空、または `elite_count` が集団サイズを超える場合は `std::invalid_argument` を送出する。
- `island_model_genetic_algorithm` では島は 1 個以上で、各島は空であってはならない。`elite_count` または `migrants_per_epoch` がいずれかの島のサイズを超える場合は `std::invalid_argument` を送出する。
- 目的関数と遺伝的操作は呼び出し中に有効でなければならず、比較関数は一貫した順序を与えなければならない。
- 結果の再現には、同じ乱数生成器の状態・入力・標準ライブラリの実装が必要である。大域最適性は保証しない。
