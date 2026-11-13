---
title: Advanced Neighborhood Search (高度な近傍探索)
documentation_of: ../../../src/approximate/metaheuristic/advanced_neighborhood_search.hpp
---

`approximate::metaheuristic` 名前空間で、最小化問題向けの高度な近傍制御を提供する。いずれの関数も訪れた最良の状態を `SearchResult<State, Score>` で返し、乱数が必要なものは外部の乱数生成器（URBG）を受け取る。

以下、反復回数を $I$、評価 1 回のコストを $E$ とする。

## `skewed_variable_neighborhood_search`

```cpp
skewed_variable_neighborhood_search(
    initial, shakings, local_improve, evaluate, distance,
    iterations, distance_weight, rng, compare = {}
)
```

近傍を順にシェイキングし、候補 $x'$ が $f(x')-\alpha d(x,x')<f(x)$ を満たせば遠い候補も受理して最初の近傍に戻る Skewed VNS である（$\alpha$ は `distance_weight`）。実際の最良値は `compare` で追跡する。距離と `distance_weight` は有限な非負値でなければならない。

シェイキング・局所改善・距離計算のコストを $S, L, D$ とすると、時間計算量は $O(L+E+I(S+L+E+D))$、追加空間計算量はコールバックの結果を除いて $O(1)$ である。

## `guided_local_search`

```cpp
guided_local_search(
    initial, neighborhood, evaluate, features,
    iterations, penalty_weight, hash = {}
)
```

`features(state)` は（キー, 非負のコスト）の `pair` の range を返す。各反復で $f(x)+\lambda\sum_i p_i c_i(x)$ が最小の近傍へ移り、現在解で $c_i/(1+p_i)$ が最大の特徴のペナルティ $p_i$ を増やす Guided Local Search である（$\lambda$ は `penalty_weight`）。本来の目的値での最良解を返す。`penalty_weight` と特徴のコストは有限な非負値でなければならない。

反復 $t$ の近傍数を $N_t$、特徴の列挙のコストを $F$、相異なる特徴の数を $K$ とすると、期待時間計算量は $O(E+\sum_t N_t(E+F)+IF)$、追加空間計算量は $O(K+\max_tN_t)$ である。ハッシュが衝突する場合、ペナルティの操作は最悪 $O(K)$ になる。

## `iterated_greedy`

```cpp
iterated_greedy(
    initial, destroy, greedy_reconstruct, evaluate,
    iterations, rng, compare = {}
)
```

現在解の一部を破壊し、貪欲な再構築で完全な解に戻し、改善した場合だけ現在解にする Iterated Greedy である。破壊・再構築のコストを $D, R$ とすると、時間計算量は $O(E+I(D+R+E))$、追加空間計算量は部分解と完全な状態が各 1 個分である。

## `step_counting_hill_climbing`

```cpp
step_counting_hill_climbing(
    initial, random_neighbor, evaluate, iterations,
    steps_per_level, rng, compare = {}
)
```

現在の閾値以下の候補を受理し、`steps_per_level` 回受理するたびに閾値を現在値に更新する Step Counting Hill Climbing である。候補生成のコストを $G$ とすると、時間計算量は $O(E+I(G+E))$、追加空間計算量は $O(1)$ である。

## `strategic_oscillation`

```cpp
strategic_oscillation(
    initial, neighborhood, evaluate, violation,
    iterations, initial_penalty, penalty_step
)
```

$f(x)+\lambda \cdot \mathrm{violation}(x)$ が最小の近傍へ毎回移り、選んだ解が実行不能なら $\lambda$ を増やし、実行可能なら減らすことで、実行可能領域の境界付近を往復する。`violation` の値と `initial_penalty`, `penalty_step` は有限な非負値でなければならない。

違反量の計算のコストを $V$ とすると、時間計算量は $O(E+\sum_tN_t(E+V))$、追加空間計算量は $O(\max_tN_t)$ である。

## `extremal_optimization`

```cpp
extremal_optimization(
    initial, components, component_fitness, mutate_component,
    evaluate, iterations, tau, rng, compare = {}
)
```

`components(state)` を成分の適応度の悪い順に並べ、順位 $r$ を $r^{-\tau}$ に比例する確率で選び、その成分を必ず変異させる Extremal Optimization である。`tau` は有限な正の値でなければならない。

反復 $t$ の成分数を $C_t$、適応度計算のコストを $Q$、変異のコストを $M$ とすると、時間計算量は $O(E+\sum_t(C_t\log C_t\,Q+M+E))$、追加空間計算量は $O(\max_tC_t)$ である。

## 共通操作の計算量

`SearchResult` の `state`, `score`, `iterations`, `evaluations`, `accepted_moves` の各参照は時間 $O(1)$、追加空間 $O(1)$ である。

## 注意点

- パラメータやコールバックの返す値が上記の条件を満たさない場合（`steps_per_level == 0` を含む）は `std::invalid_argument` を送出する
- デフォルトは最小化である。Skewed VNS の受理式、GLS、Strategic Oscillation は値を `long double` に変換して使うため、任意の `Compare` による最大化には対応しない
- コールバックが返す range は、その走査の間有効でなければならない
- GLS の `Hash` は特徴の同一性と整合していなければならない
- いずれもヒューリスティックであり、有限の計算資源の範囲での最適性や収束は保証しない
