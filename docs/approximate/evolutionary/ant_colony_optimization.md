---
title: Ant Colony Optimization Variants (蟻コロニー最適化法)
documentation_of: ../../../src/approximate/evolutionary/ant_colony_optimization.hpp
---

動的な構築状態に対する汎用 pheromone callback model。
`choices(state)` は move range、`apply(state, move)` は次状態、
`key(state, move)` は pheromone key、`heuristic` は有限非負 desirability、
`complete(state)` は完成判定を返す。

## `AntColonyResult`

`solution`, `score`, final `pheromones`, `iterations`, `evaluations` を持つ。
scalar member 参照は $O(1)$、pheromone map の lookup は期待 $O(1)$、
コピーは key 数 $K$ に対し $O(K)$。

## `ant_colony_optimization`

```cpp
ant_colony_optimization<Real>(
    initial, complete, choices, apply, heuristic, key, evaluate, update,
    iterations, ants, maximum_steps, initial_pheromone,
    pheromone_exponent, heuristic_exponent, rng, hash = {}, compare = {}
)
```

確率 $\tau^\alpha\eta^\beta$ で move を選ぶ汎用 ACO。
各世代後 `update(pheromones, solutions, scores, paths, iteration)` を呼ぶ。
This callback is the explicit policy point; updated pheromones must remain
finite and positive.

## `ant_system`

全 pheromone を evaporation し、全 ant の path に `deposit(score)` を加える
Ant System。evaporation は $(0,1]$、deposit は有限非負。

## `ant_colony_system`

選択 edge に local update
$(1-\xi)\tau+\xi\tau_0$、各世代後に global-best path だけを強化する ACS。
local/global rate は $(0,1]$。

## `max_min_ant_system`

iteration-best path だけを強化し、その後全 pheromone を
`[minimum_pheromone, maximum_pheromone]` に clamp する MAX–MIN Ant System。

## 計算量

反復 $I$、ant 数 $A$、最大構築長 $S$、step $t$ の選択肢数 $B_t$、
評価 $E$ とする。4 API の時間計算量は
$O(IA(\sum_{t<S}B_t+E)+U)$、追加空間は
$O(K+AS+A)$。`U` は policy update で、Ant System は全 path 長、
ACS は $O(K+S)$、MAX–MIN は $O(K+A+S)$。

## 注意点

- `iterations`, `ants`, `maximum_steps` は正。初期 pheromone は有限正、
  exponent は有限非負。
- dead end または step 上限内に完成しない構築は `std::runtime_error`。
- `Hash` は key の等値関係と整合させる。hash 衝突時の map 操作は最悪 $O(K)$。
- `deposit(score)` の尺度は問題依存であり、ゼロ除算を callback 内で避ける。
