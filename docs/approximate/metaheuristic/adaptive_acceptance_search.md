---
title: Adaptive Acceptance and Tempering (適応的受理と温度交換法)
documentation_of: ../../../src/approximate/metaheuristic/adaptive_acceptance_search.hpp
---

最小化問題に対する適応的焼きなまし、探索半径の適応、温度交換による探索を提供する。乱数を使う API はすべて外部の乱数生成器（URBG）を受け取り、訪れた最良解を `SearchResult` で返す。

以下、反復回数を $I$、候補生成 1 回のコストを $G$、評価 1 回のコストを $E$ とする。

## `adaptive_simulated_annealing`

```cpp
adaptive_simulated_annealing(
    initial, random_neighbor, evaluate, iterations,
    initial_temperature, adaptation_interval,
    target_acceptance_rate, adaptation_factor, rng
)
```

Metropolis 基準で受理を行い、`adaptation_interval` 反復ごとの区間の受理率が `target_acceptance_rate` 未満なら温度を `adaptation_factor` 倍し、それ以外なら `adaptation_factor` で割る適応的焼きなまし法である。温度は有限な正の値、区間長は正、目標受理率は $[0,1]$、`adaptation_factor` は有限で 1 より大きくなければならない。

時間計算量は $O(E+I(G+E))$、追加空間計算量は $O(1)$ である。

## `adaptive_random_search`

```cpp
adaptive_random_search(
    generator, evaluate, samples, initial_step,
    expansion_factor, contraction_factor, failure_limit,
    rng, compare = {}
)
```

`generator(step, rng)` で標本を作り、改善したら `step` を `expansion_factor` 倍に拡大し、連続失敗が `failure_limit` 回に達したら `contraction_factor` 倍に縮小する。`samples` は正、`initial_step` は正、`expansion_factor` は 1 より大、`contraction_factor` は $(0,1)$、`failure_limit` は正でなければならない。

標本数を $S$ とすると、時間計算量は $O(S(G+E))$、追加空間計算量は状態 2 個分である。

## `parallel_tempering`

```cpp
parallel_tempering(
    replicas, temperatures, random_neighbor, evaluate,
    iterations, exchange_interval, rng
)
```

各温度で独立に Metropolis 遷移を行い、`exchange_interval` 反復ごとに隣接温度のレプリカを詳細釣り合いを満たす確率で交換する温度交換法（Parallel Tempering）である。`replicas` と `temperatures` は同じ長さで空でなく、温度は有限な正の値、`exchange_interval` は正でなければならない。

レプリカ数を $R$、$X = \texttt{exchange_interval}$ とすると、時間計算量は $O(RE+IR(G+E)+IR/X)$、追加空間計算量は $O(R)$ 個の状態と目的値である。

## `simulated_tempering`

```cpp
simulated_tempering(
    initial, temperatures, random_neighbor, evaluate,
    iterations, temperature_move_probability, rng
)
```

単一のレプリカについて、確率 `temperature_move_probability` で隣接する温度レベルへの変更を提案し、それ以外は現在の温度で Metropolis 遷移を行う Simulated Tempering である。確率は $[0,1]$、温度列は空でなく、各温度は有限な正の値でなければならない。

時間計算量は $O(E+I(G+E))$、追加空間計算量は $O(1)$（温度の入力を除く）である。

## 共通操作の計算量

`SearchResult` のメンバの参照は時間・追加空間とも $O(1)$ である。

## 注意点

- パラメータが上記の条件を満たさない場合は `std::invalid_argument` を送出する
- 目的値は有限で、`long double` に変換可能でなければならない
- Simulated Tempering の温度レベル間の移動は、一様な目標重みを仮定する。非一様な分配関数の重みが必要なら、目的値の側に補正を含める
- `adaptive_random_search` の `Step` は算術型でなければならない（`static_assert`）。拡大で表現範囲を超える場合は拡大せず、縮小で 0 以下になる場合は `std::numeric_limits<Step>::min()` にする
- 温度を使う手法は確率的なヒューリスティックであり、有限回の実行ですべての峰を訪れることは保証しない
