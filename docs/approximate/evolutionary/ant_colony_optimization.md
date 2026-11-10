---
title: Ant Colony Optimization Variants (蟻コロニー最適化法)
documentation_of: ../../../src/approximate/evolutionary/ant_colony_optimization.hpp
---

動的に構築される状態に対する、汎用的なフェロモンのコールバックモデルである。`choices(state)` は選択肢（move）の range、`apply(state, move)` は次の状態、`key(state, move)` はフェロモンのキー、`heuristic` は有限かつ非負の望ましさ、`complete(state)` は構築が完了したかを返す。

## `AntColonyResult`

最良解 `solution`、その評価値 `score`、最終的なフェロモン表 `pheromones`、反復回数 `iterations`、評価回数 `evaluations` を持つ。スカラーのメンバの参照は $O(1)$、フェロモン表の検索は期待 $O(1)$、そのコピーはキーの個数 $K$ に対して $O(K)$ である。

## `ant_colony_optimization`

```cpp
ant_colony_optimization<Real>(
    initial, complete, choices, apply, heuristic, key, evaluate, update,
    iterations, ants, maximum_steps, initial_pheromone,
    pheromone_exponent, heuristic_exponent, rng, hash = {}, compare = {}
)
```

$\tau^\alpha\eta^\beta$ に比例する確率で選択肢を選ぶ汎用の ACO である。各世代の後に `update(pheromones, solutions, scores, paths, iteration)` を呼ぶ。このコールバックがフェロモン更新の方針を明示的に決める箇所であり、更新後のフェロモンは正の有限値でなければならない。

## `ant_system`

```cpp
ant_system<Real>(
    initial, complete, choices, apply, heuristic, key, evaluate, deposit,
    iterations, ants, maximum_steps, initial_pheromone, evaporation_rate,
    pheromone_exponent, heuristic_exponent, rng, hash = {}, compare = {}
)
```

全てのフェロモンを蒸発させ、全ての蟻の経路に `deposit(score)` を加える Ant System である。蒸発率は $(0,1]$、`deposit` は有限かつ非負でなければならない。

## `ant_colony_system`

```cpp
ant_colony_system<Real>(
    initial, complete, choices, apply, heuristic, key, evaluate, deposit,
    iterations, ants, maximum_steps, initial_pheromone,
    global_evaporation_rate, local_update_rate,
    pheromone_exponent, heuristic_exponent, rng, hash = {}, compare = {}
)
```

選んだ辺に局所更新 $(1-\xi)\tau+\xi\tau_0$ を行い、各世代の後にこれまでの最良経路だけを強化する ACS である。局所・大域の更新率は $(0,1]$ でなければならない。

## `max_min_ant_system`

```cpp
max_min_ant_system<Real>(
    initial, complete, choices, apply, heuristic, key, evaluate, deposit,
    iterations, ants, maximum_steps, initial_pheromone,
    minimum_pheromone, maximum_pheromone, evaporation_rate,
    pheromone_exponent, heuristic_exponent, rng, hash = {}, compare = {}
)
```

その世代の最良経路だけを強化し、その後全てのフェロモンを `[minimum_pheromone, maximum_pheromone]` に収める MAX–MIN Ant System である。`0 < minimum_pheromone <= maximum_pheromone`、蒸発率は $(0,1]$ でなければならない。

## 計算量

反復回数を $I$、蟻の数を $A$、構築長の上限を $S$、ステップ $t$ の選択肢数を $B_t$、評価 1 回のコストを $E$、フェロモン更新のコストを $U$ とする。4 つの API の時間計算量は $O(IA(\sum_{t<S}B_t+E)+IU)$、追加空間計算量は $O(K+AS+A)$ である。$U$ は、Ant System では全経路長の和、ACS では $O(K+S)$、MAX–MIN では $O(K+A+S)$ である。

## 注意点

- `iterations`、`ants`、`maximum_steps` は正でなければならない。初期フェロモンは正の有限値、指数は非負の有限値でなければならない。違反時は `std::invalid_argument` を送出する。
- 行き止まりに達した場合や、ステップ数の上限内に構築が完了しない場合は `std::runtime_error` を送出する。
- `Hash` はキーの等価関係と整合していなければならない。ハッシュが衝突すると、表の操作は最悪 $O(K)$ になる。
- `deposit(score)` の尺度は問題に依存する。ゼロ除算はコールバック内で避ける。
