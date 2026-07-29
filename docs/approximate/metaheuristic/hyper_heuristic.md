---
title: Selection Hyper-Heuristics (選択型ハイパーヒューリスティクス)
documentation_of: ../../../src/approximate/metaheuristic/hyper_heuristic.hpp
---

`heuristics` は同じ callable 型の range で、各要素は
`heuristic(current, rng) -> State` を満たす。Both APIs minimize by default,
accept only improving moves, and return the best `SearchResult`.

## `hyper_heuristic`

```cpp
hyper_heuristic(
    initial, heuristics, evaluate, iterations, rng, compare = {}
)
```

各反復で low-level heuristic を一様ランダムに一つ選ぶ selection
Hyper-Heuristic。operator 数を $H$、適用を $A$、評価を $E$ とすると
時間計算量 $O(E+I(H+A+E))$。forward iterator から選択位置まで進むため $H$
を含み、random-access range でも現在の API は同じ。追加空間 $O(1)$。

## `choice_function_hyper_heuristic`

```cpp
choice_function_hyper_heuristic(
    initial, heuristics, evaluate, iterations,
    reaction_factor, recency_weight, transition_weight,
    rng, compare = {}
)
```

operator 自身の exponential reward、未使用期間、直前 operator からの
transition reward の和が最大の operator を選ぶ Choice Function
Hyper-Heuristic。改善を reward 1、非改善を 0 として更新する。
operator 群は非空、reaction は $(0,1]$、二つの weight は非負。

時間計算量 $O(E+I(H+A+E))$、追加空間は transition table の $O(H^2)$。

## 共通操作の計算量 / Common operations

`SearchResult` の各 member 参照は時間・追加空間 $O(1)$。

## 注意点

- operator range は探索中変更せず、各 callable の戻り状態は独立でなければならない。
- choice function の reward は score の改善量でなく改善の有無。尺度を考慮した
  reward が必要なら low-level operator 側で正規化する。
- The random baseline and learned choice function are finite-budget
  heuristics and provide no global-optimality guarantee.
