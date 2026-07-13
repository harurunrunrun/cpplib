---
title: Acceptance-Based Metaheuristics (受理基準型メタヒューリスティクス)
documentation_of: ../src/approximate/metaheuristic/acceptance_search.hpp
---

`approximate::metaheuristic`名前空間で、悪化遷移も許す最小化探索を提供する。
目的値`Score`は`long double`へ変換でき、大小比較可能でなければならない。
各関数は探索中に訪れた最良解を返す。乱数器は参照で渡すためseedを再現できる。

## `simulated_annealing`

```cpp
auto result = simulated_annealing(
    initial, random_neighbor, evaluate, iterations,
    initial_temperature, cooling_rate, rng
);
```

温度$T_i=T_0r^i$を使う焼きなまし法である。悪化量$\Delta$を確率
$\exp(-\Delta/T_i)$で受理する。$T_0>0$, $0<r\leq1$が必要である。

## `fast_simulated_annealing`

温度$T_i=T_0/(i+1)$を使う。`initial_temperature`は正でなければならない。

## `threshold_accepting`

```cpp
auto result = threshold_accepting(
    initial, random_neighbor, evaluate, iterations,
    initial_threshold, cooling_rate, rng
);
```

悪化量が閾値以下なら受理し、閾値を反復ごとに`cooling_rate`倍する。
閾値は非負、率は$[0,1]$でなければならない。

## `great_deluge`

```cpp
auto result = great_deluge(
    initial, random_neighbor, evaluate, iterations,
    initial_water_level, decay, rng
);
```

改善解または目的値がwater level以下の解を受理し、levelを毎回`decay`だけ下げる。
`decay`は非負でなければならない。

## `late_acceptance_hill_climbing`

```cpp
auto result = late_acceptance_hill_climbing(
    initial, random_neighbor, evaluate, iterations, history_length, rng
);
```

現在値または`history_length`反復前の値以下なら受理する。
履歴長0では`std::invalid_argument`を送出する。

## 計算量

$I$を反復数、候補生成1回を$G$、評価1回を$E$とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `simulated_annealing` | $O(I(G+E))$ | $O(1)$ |
| `fast_simulated_annealing` | $O(I(G+E))$ | $O(1)$ |
| `threshold_accepting` | $O(I(G+E))$ | $O(1)$ |
| `great_deluge` | $O(I(G+E))$ | $O(1)$ |
| `late_acceptance_hill_climbing` | $O(I(G+E))$ | $O(\mathrm{history\_length})$ |
