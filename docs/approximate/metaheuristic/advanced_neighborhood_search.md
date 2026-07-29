---
title: Advanced Neighborhood Search (高度な近傍探索)
documentation_of: ../../../src/approximate/metaheuristic/advanced_neighborhood_search.hpp
---

`approximate::metaheuristic` 名前空間で、最小化問題向けの高度な近傍制御を
提供する。All functions return the best visited state as
`SearchResult<State, Score>` and accept an external URBG where randomness is
needed.

## `skewed_variable_neighborhood_search`

```cpp
skewed_variable_neighborhood_search(
    initial, shakings, local_improve, evaluate, distance,
    iterations, distance_weight, rng, compare = {}
)
```

近傍を順に shaking し、候補 $x'$ が
$f(x')-\alpha d(x,x')<f(x)$ を満たせば遠い候補も受理して近傍 0 へ戻る
Skewed VNS。実際の最良値は `compare` で追跡する。距離と
`distance_weight` は有限非負でなければならない。

With $I$ iterations, shaking cost $S$, local improvement $L$, evaluation
$E$, and distance cost $D$, the time complexity is
$O(L+E+I(S+L+E+D))$ and auxiliary space is $O(1)$ plus callback results.

## `guided_local_search`

```cpp
guided_local_search(
    initial, neighborhood, evaluate, features,
    iterations, penalty_weight, hash = {}
)
```

`features(state)` は `pair<Key, nonnegative_cost>` の range を返す。
各反復で $f(x)+\lambda\sum_i p_i c_i(x)$ が最小の近傍へ移り、現在解で
$c_i/(1+p_i)$ が最大の feature の penalty を増やす Guided Local Search。
実目的値の最良解を返す。

反復 $t$ の近傍数を $N_t$、feature 列挙を $F$ とすると、期待時間計算量は
$O(E+\sum_t N_t(E+F)+IF)$、追加空間は異なる feature 数を $K$ として
$O(K+\max_tN_t)$。hash 衝突時は penalty 操作が最悪 $O(K)$。

## `iterated_greedy`

```cpp
iterated_greedy(
    initial, destroy, greedy_reconstruct, evaluate,
    iterations, rng, compare = {}
)
```

現在解を一部破壊し、greedy reconstruction で完全解へ戻し、改善解だけを
現在解にする Iterated Greedy。Time is $O(E+I(D+R+E))$ and auxiliary
space is one partial and one complete state.

## `step_counting_hill_climbing`

```cpp
step_counting_hill_climbing(
    initial, random_neighbor, evaluate, iterations,
    steps_per_level, rng, compare = {}
)
```

現在 threshold 以下の候補を受理し、`steps_per_level` 回受理するたび threshold
を現在値へ更新する Step Counting Hill Climbing。`steps_per_level == 0` は
`std::invalid_argument`。時間計算量 $O(E+I(G+E))$、追加空間 $O(1)$。

## `strategic_oscillation`

```cpp
strategic_oscillation(
    initial, neighborhood, evaluate, violation,
    iterations, initial_penalty, penalty_step
)
```

$f(x)+\lambda\,violation(x)$ 最小の近傍へ毎回移り、選択解が infeasible なら
$\lambda$ を増やし、feasible なら減らすことで境界を往復する。
`violation` と penalty は有限非負が必要。

近傍数を $N_t$、評価を $E$、違反量計算を $V$ とすると時間計算量
$O(E+\sum_tN_t(E+V))$、追加空間 $O(\max_tN_t)$。

## `extremal_optimization`

```cpp
extremal_optimization(
    initial, components, component_fitness, mutate_component,
    evaluate, iterations, tau, rng, compare = {}
)
```

`components(state)` を component fitness の悪い順に並べ、rank $r$ を
$r^{-\tau}$ に比例して選び、その component を必ず mutate する Extremal
Optimization。`tau` は有限正。

component 数を $C_t$、fitness 計算を $Q$、mutation を $M$ とすると
時間計算量 $O(E+\sum_t(C_t\log C_t\,Q+M+E))$、追加空間
$O(\max_tC_t)$。

## 共通操作の計算量 / Common operations

`SearchResult` の `state`, `score`, `iterations`, `evaluations`,
`accepted_moves` の各参照は時間 $O(1)$、追加空間 $O(1)$。

## 注意点

- 既定は最小化。Skewed VNS の受理式、GLS、Strategic Oscillation は数値を
  `long double` に変換するため、任意 `Compare` による最大化には対応しない。
- callback が返す range はその走査中有効でなければならない。
- GLS の `Hash` は feature の同一性と整合しなければならない。
- These are heuristics: finite resource limits do not imply optimality or
  convergence.
