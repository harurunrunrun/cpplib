---
title: Hill Climbing and Random Search (山登り法とランダム探索)
documentation_of: ../../../src/approximate/metaheuristic/hill_climbing.hpp
---

`approximate::metaheuristic` 名前空間で、目的値を最小化する汎用の局所探索を提供する。`evaluate(state)` は目的値、`neighborhood(state)` は候補解の range、`random_neighbor(state, rng)` はランダムな候補解を返す。デフォルトの `Compare` は `std::less<>` であり、変更すれば最大化にも使える。`State` はコピー・ムーブ可能でなければならない。

## `best_improvement_hill_climbing`

```cpp
auto result = best_improvement_hill_climbing(
    initial, neighborhood, evaluate, maximum_iterations, compare = {}
);
```

各反復で近傍全体を評価し、最良の改善解へ遷移する。改善解がなければ終了する。

## `first_improvement_hill_climbing`

```cpp
auto result = first_improvement_hill_climbing(
    initial, neighborhood, evaluate, maximum_iterations, compare = {}
);
```

列挙順で最初に見つかった改善解へ遷移する。改善解がなければ終了する。

## `stochastic_hill_climbing`

```cpp
auto result = stochastic_hill_climbing(
    initial, random_neighbor, evaluate, iterations, rng, compare = {}
);
```

各反復で候補を 1 つ生成し、改善するときだけ遷移する。

## `random_search`

```cpp
auto result = random_search(generator, evaluate, samples, rng, compare = {});
```

`generator(rng)` で独立に生成した `samples` 個の解のうち最良のものを返す。

## `random_restart_hill_climbing`

```cpp
auto result = random_restart_hill_climbing(
    generator, neighborhood, evaluate,
    restarts, maximum_iterations_per_restart, rng, compare = {}
);
```

`generator(rng)` で作った `restarts` 個の初期解それぞれから最良改善の山登り法を実行し、全試行の最良解を返す。`iterations`, `evaluations`, `accepted_moves` は全試行の合計である。

## 計算量

$I$ を実行した反復回数、反復 $i$ の近傍数を $D_i$、目的関数 1 回のコストを $E$、候補生成 1 回のコストを $G$、$S = \texttt{samples}$ とする（状態のコピーのコストを除く）。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `best_improvement_hill_climbing` | $O(E(1+\sum_iD_i))$ | $O(\max_iD_i)$ |
| `first_improvement_hill_climbing` | $O(E(1+\sum_iD_i))$（上限） | $O(\max_iD_i)$ |
| `stochastic_hill_climbing` | $O(E+I(E+G))$ | $O(1)$ |
| `random_search` | $O(S(E+G))$ | $O(1)$ |
| `random_restart_hill_climbing` | 各試行の最良改善探索の合計 | $O(\max_iD_i)$ |

## 注意点

- `samples == 0` や `restarts == 0` では `std::invalid_argument` を送出する
- 目的関数・近傍生成器が返す値と range は、その呼び出しの間有効でなければならない
- 探索はヒューリスティックであり、明記した停止条件を除いて大域最適性や収束は保証しない
