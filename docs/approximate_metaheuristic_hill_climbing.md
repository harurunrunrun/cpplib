---
title: Hill Climbing and Random Search (山登り法とランダム探索)
documentation_of: ../src/approximate/metaheuristic/hill_climbing.hpp
---

`approximate::metaheuristic`名前空間で、目的値を最小化する汎用局所探索を提供する。
`evaluate(state)`は目的値、`neighborhood(state)`は候補解のrange、
`random_neighbor(state, rng)`はランダムな候補解を返す。
既定の`Compare`は`std::less<>`であり、変更すれば最大化にも利用できる。
`State`はコピー・move可能でなければならない。

## `best_improvement_hill_climbing`

```cpp
auto result = best_improvement_hill_climbing(
    initial, neighborhood, evaluate, maximum_iterations, compare
);
```

各反復で近傍全体を評価し、最良の改善解へ遷移する。改善解がなければ終了する。

## `first_improvement_hill_climbing`

同じ引数を取り、列挙順で最初の改善解へ遷移する。

## `stochastic_hill_climbing`

```cpp
auto result = stochastic_hill_climbing(
    initial, random_neighbor, evaluate, iterations, rng, compare
);
```

各反復で1候補を生成し、改善時だけ遷移する。

## `random_search`

```cpp
auto result = random_search(generator, evaluate, samples, rng, compare);
```

`generator(rng)`で独立に生成した解の最良値を返す。`samples == 0`では
`std::invalid_argument`を送出する。

## `random_restart_hill_climbing`

```cpp
auto result = random_restart_hill_climbing(
    generator, neighborhood, evaluate,
    restarts, maximum_iterations_per_restart, rng, compare
);
```

複数の初期解から最良改善山登りを実行し、全試行の最良解を返す。
`restarts == 0`では`std::invalid_argument`を送出する。

## 計算量

$I$を実行反復数、反復$i$の近傍数を$D_i$、目的関数1回を$E$、
候補生成1回を$G$とする（状態コピーの費用を除く）。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `best_improvement_hill_climbing` | $O(E(1+\sum_iD_i))$ | $O(\max_iD_i)$ |
| `first_improvement_hill_climbing` | $O(E(1+\sum_iD_i))$（上限） | $O(\max_iD_i)$ |
| `stochastic_hill_climbing` | $O(E+I(E+G))$ | $O(1)$ |
| `random_search` | $O(\mathrm{samples}(E+G))$ | $O(1)$ |
| `random_restart_hill_climbing` | 各restartの最良改善探索の合計 | $O(\max_iD_i)$ |
