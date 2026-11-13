---
title: Acceptance-Based Metaheuristics (受理基準型メタヒューリスティクス)
documentation_of: ../../../src/approximate/metaheuristic/acceptance_search.hpp
---

`approximate::metaheuristic` 名前空間で、悪化する遷移も受理し得る最小化探索を提供する。目的値 `Score` は `long double` に変換でき、大小比較が可能でなければならない。各関数は探索中に訪れた最良解を `SearchResult` で返す。乱数生成器は参照で渡すので、シードを固定すれば結果を再現できる。

## `simulated_annealing`

```cpp
auto result = simulated_annealing(
    initial, random_neighbor, evaluate, iterations,
    initial_temperature, cooling_rate, rng
);
```

温度 $T_i=T_0r^i$ を使う焼きなまし法である。悪化量 $\Delta$ の候補を確率 $\exp(-\Delta/T_i)$ で受理する。$T_0>0$ かつ $0<r\le 1$ が必要である。温度は `long double` の最小正規化数を下限とし、0 にはならない。

## `fast_simulated_annealing`

```cpp
auto result = fast_simulated_annealing(
    initial, random_neighbor, evaluate, iterations,
    initial_temperature, rng
);
```

温度 $T_i=T_0/(i+1)$ を使う焼きなまし法である。`initial_temperature` は正でなければならない。

## `threshold_accepting`

```cpp
auto result = threshold_accepting(
    initial, random_neighbor, evaluate, iterations,
    initial_threshold, cooling_rate, rng
);
```

悪化量が閾値以下なら受理し、閾値を反復ごとに `cooling_rate` 倍する。閾値は非負、`cooling_rate` は $[0,1]$ でなければならない。

## `great_deluge`

```cpp
auto result = great_deluge(
    initial, random_neighbor, evaluate, iterations,
    initial_water_level, decay, rng
);
```

改善解、または目的値が水位（water level）以下の解を受理し、水位を毎回 `decay` だけ下げる。`decay` は非負でなければならない。

## `late_acceptance_hill_climbing`

```cpp
auto result = late_acceptance_hill_climbing(
    initial, random_neighbor, evaluate, iterations, history_length, rng
);
```

候補の目的値が現在値以下、または `history_length` 反復前の値以下なら受理する。

## 計算量

$I$ を反復回数、候補生成 1 回のコストを $G$、評価 1 回のコストを $E$、$H = \texttt{history_length}$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `simulated_annealing` | $O(I(G+E))$ | $O(1)$ |
| `fast_simulated_annealing` | $O(I(G+E))$ | $O(1)$ |
| `threshold_accepting` | $O(I(G+E))$ | $O(1)$ |
| `great_deluge` | $O(I(G+E))$ | $O(1)$ |
| `late_acceptance_hill_climbing` | $O(I(G+E))$ | $O(H)$ |

## 注意点

- パラメータが上記の条件を満たさない場合（`history_length == 0` を含む）は `std::invalid_argument` を送出する
- 目的関数・近傍生成器が返す値と range は、その呼び出しの間有効でなければならない
- 探索はヒューリスティックであり、明記した停止条件を除いて大域最適性や収束は保証しない
