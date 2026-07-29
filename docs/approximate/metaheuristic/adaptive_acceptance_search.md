---
title: Adaptive Acceptance and Tempering (適応的受理と温度交換法)
documentation_of: ../../../src/approximate/metaheuristic/adaptive_acceptance_search.hpp
---

最小化問題に対する適応的焼きなまし、探索半径適応、温度交換探索を提供する。
Every randomized API takes an external URBG and returns the best visited
`SearchResult`.

## `adaptive_simulated_annealing`

```cpp
adaptive_simulated_annealing(
    initial, random_neighbor, evaluate, iterations,
    initial_temperature, adaptation_interval,
    target_acceptance_rate, adaptation_factor, rng
)
```

Metropolis 受理を行い、各 window の受理率が target 未満なら温度を
`adaptation_factor` 倍、それ以外なら同値で割る Adaptive SA。
温度は有限正、interval は正、target は $[0,1]$、factor は有限で 1 より大。
候補生成を $G$、評価を $E$ とすると時間計算量 $O(E+I(G+E))$、
追加空間 $O(1)$。

## `adaptive_random_search`

```cpp
adaptive_random_search(
    generator, evaluate, samples, initial_step,
    expansion_factor, contraction_factor, failure_limit,
    rng, compare = {}
)
```

`generator(step, rng)` で標本を作り、改善時は step を拡大し、連続失敗が
`failure_limit` に達すると縮小する。標本数は正、step は正、
expansion は 1 より大、contraction は $(0,1)$。

$S$ samples、生成を $G$、評価を $E$ とすると時間計算量
$O(S(G+E))$、追加空間は状態 2 個分。

## `parallel_tempering`

```cpp
parallel_tempering(
    replicas, temperatures, random_neighbor, evaluate,
    iterations, exchange_interval, rng
)
```

各温度で独立に Metropolis move を行い、interval ごとに隣接温度の replica を
詳細釣合いの確率で交換する Parallel Tempering。replica と温度は同じ非ゼロ数、
温度は有限正、interval は正。

replica 数 $R$ とすると時間計算量
$O(RE+IR(G+E)+I R/exchange\_interval)$、追加空間 $O(R)$ 個の状態と score。

## `simulated_tempering`

```cpp
simulated_tempering(
    initial, temperatures, random_neighbor, evaluate,
    iterations, temperature_move_probability, rng
)
```

単一 replica について、指定確率で隣接温度 level の変更を提案し、それ以外は
現在温度で Metropolis move を行う Simulated Tempering。確率は $[0,1]$、
温度列は非空かつ有限正。

時間計算量 $O(E+I(G+E))$、追加空間 $O(1)$（温度入力を除く）。

## 共通操作の計算量 / Common operations

`SearchResult` member 参照は時間・追加空間とも $O(1)$。

## 注意点

- score は有限で `long double` に変換可能でなければならない。
- Simulated Tempering の温度 level move は uniform target weights を仮定する。
  非一様な partition-function weight が必要なら score 側へ補正を含める。
- `adaptive_random_search` の `Step` は実数型を想定する。乗算が表現範囲を
  超えないよう設定する。
- Temperature methods are stochastic heuristics and do not guarantee that
  every mode is visited within a finite run.
