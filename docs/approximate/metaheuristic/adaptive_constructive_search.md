---
title: Adaptive Constructive Metaheuristics (適応的構築メタヒューリスティクス)
documentation_of: ../../../src/approximate/metaheuristic/adaptive_constructive_search.hpp
---

Tabu tenure、GRASP parameter、destroy/repair operator、分布 parameter などを
探索中に制御する汎用最小化 API。Callbacks define the problem representation;
the library owns only the metaheuristic control logic.

## `reactive_tabu_search`

```cpp
reactive_tabu_search(
    initial, neighborhood, evaluate, key, iterations,
    initial_tenure, minimum_tenure, maximum_tenure,
    decrease_interval, hash = {}, compare = {}
)
```

再訪を検出すると tenure を 1 増やし、cycle のない interval ごとに 1 減らす
Reactive Tabu Search。全体最良を改善する tabu move には aspiration を適用する。
tenure は $1\le min\le initial\le max$、interval は正。

反復 $t$ の近傍数 $N_t$、評価 $E$ とすると期待時間計算量
$O(E+\sum_tN_tE)$、追加空間 $O(I+\max_tN_t)$。hash 最悪時は
$O(I\sum_tN_t)$。

## `granular_tabu_search`

```cpp
granular_tabu_search(
    initial, neighborhood, evaluate, key, granular_filter,
    iterations, tabu_tenure, hash = {}, compare = {}
)
```

`granular_filter(current, candidate)` が真の疎な近傍だけを評価して Tabu Search
する。tenure は正。filter を $F$ とすると期待時間計算量
$O(\sum_tN_tF+E\sum_tA_t)$（$A_t$ は通過数）、追加空間 $O(I)$。

## `reactive_grasp`

```cpp
reactive_grasp(
    construct, local_improve, evaluate, alpha_values,
    iterations, adaptation_interval, rng, compare = {}
)
```

`construct(alpha, rng)` の alpha を roulette 選択し、区間ごとの平均 score と
全体最良の差の逆数で選択 weight を更新する Reactive GRASP。alpha は
$[0,1]$、集合・反復・interval は非空／正。

alpha 数 $A$、構築 $C$、局所改善 $L$、評価 $E$ とすると時間計算量
$O(I(C+L+E+A))$、追加空間 $O(A)$。

## `large_neighborhood_search`

```cpp
large_neighborhood_search(
    initial, destroy, repair, evaluate, accept,
    iterations, rng, compare = {}
)
```

各反復で destroy/repair し、
`accept(candidate_score, current_score, iteration, rng)` が真なら現在解にする。
この明示的受理 callback により改善限定、annealing、record-to-record などを
選べる LNS。時間計算量 $O(E+I(D+R+E+A))$、追加空間は部分解と状態各 1 個。

## `adaptive_large_neighborhood_search`

```cpp
adaptive_large_neighborhood_search(
    initial, destroys, repairs, evaluate, iterations,
    reaction_factor, rng, compare = {}
)
```

destroy と repair を独立 roulette 選択し、新 best に 5、改善受理に 2、
不採用に 0.1 の reward を与え exponential smoothing する ALNS。
operator 群は非空、reaction は有限な $(0,1]$。

operator 数を $D_o,R_o$ とすると時間計算量
$O(E+I(D_o+R_o+D+R+E))$、追加空間 $O(D_o+R_o)$。

## `ruin_and_recreate`

```cpp
ruin_and_recreate(
    initial, ruin, recreate, evaluate, iterations,
    initial_ruin_size, maximum_ruin_size, rng, compare = {}
)
```

非改善ごとに ruin size を 1 増やし、改善時は初期値へ戻す Ruin and Recreate。
size は $1\le initial\le maximum$。時間計算量
$O(E+I(U+R+E))$、追加空間は部分解と状態各 1 個。

## `scatter_search`

```cpp
scatter_search(
    reference_set, combine, local_improve, evaluate,
    iterations, reference_set_size, rng, compare = {}
)
```

reference set の全 unordered pair を結合・局所改善し、親子 pool の上位を次の
reference set とする Scatter Search。入力 set と保持数は非ゼロ。

保持数 $N$、結合 $C$、局所改善 $L$、評価 $E$ とすると時間計算量
$O(NE+I(N^2(C+L+E)+N^2\log N))$、追加空間 $O(N^2)$ 個の状態。

## `pilot_method`

```cpp
pilot_method(
    initial_partial, complete, extensions, rollout,
    evaluate, maximum_steps, compare = {}
)
```

各 construction step で全 extension を rollout して評価し、最良 rollout を
与える partial extension を確定する Pilot Method。初期 partial も一度
rollout する。step $t$ の extension 数を $B_t$、rollout を $R$ とすると
時間計算量 $O(R+E+\sum_tB_t(R+E))$、追加空間 $O(\max_tB_t)$ 個の状態。

## `cross_entropy_method`

```cpp
cross_entropy_method(
    parameters, sample, evaluate, update,
    iterations, population_size, elite_count, rng, compare = {}
)
```

各反復で `sample(parameters, rng)` を population size 回呼び、上位 elite を
`update(elites, parameters)` へ渡して分布 parameter を更新する Cross-Entropy
Method。反復、population、elite は正で `elite_count <= population_size`。

population $P$、elite $K$、sampling $S$、評価 $E$、update $U$ とすると
時間計算量 $O(I(P(S+E)+P\log P+U))$、追加空間 $O(P)$ 個の状態と score。

## 共通操作の計算量 / Common operations

各 API の `SearchResult` member 参照は時間・追加空間 $O(1)$。

## 注意点

- 既定は最小化。Cross-Entropy、Scatter、GRASP は `Compare` を一貫して使う。
- ALNS の destroy 群は同じ partial 型を返し、全 repair がその型を受け取る
  callable でなければならない。
- Tabu key の `Hash` と等値関係、reference set と callback の寿命を呼出側が
  一貫して管理する。
- Objective evaluation counts do not include evaluations performed inside
  user callbacks such as rollout or local improvement.
- いずれも有限時間での最適性を保証しない。
