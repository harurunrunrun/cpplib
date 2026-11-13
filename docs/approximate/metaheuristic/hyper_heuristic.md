---
title: Selection Hyper-Heuristics (選択型ハイパーヒューリスティクス)
documentation_of: ../../../src/approximate/metaheuristic/hyper_heuristic.hpp
---

低レベルのヒューリスティック（演算子）を選んで適用する選択型ハイパーヒューリスティクスである。`heuristics` は同じ呼び出し可能型の range で、各要素は `heuristic(current, rng) -> State` を満たす。どちらの API もデフォルトでは最小化を行い、改善する遷移だけを受理し、最良解を `SearchResult` で返す。

以下、反復回数を $I$、演算子の個数を $H$、演算子の適用コストを $A$、評価 1 回のコストを $E$ とする。

## `hyper_heuristic`

```cpp
hyper_heuristic(
    initial, heuristics, evaluate, iterations, rng, compare = {}
)
```

各反復で低レベルのヒューリスティックを一様ランダムに 1 つ選ぶ選択型ハイパーヒューリスティクスである。時間計算量は $O(E+I(H+A+E))$、追加空間計算量は $O(1)$ である。選んだ位置まで前方向イテレータで進めるため $H$ の項を含み、ランダムアクセス可能な range でも現在の実装では同じである。

## `choice_function_hyper_heuristic`

```cpp
choice_function_hyper_heuristic(
    initial, heuristics, evaluate, iterations,
    reaction_factor, recency_weight, transition_weight,
    rng, compare = {}
)
```

演算子自身の指数平滑化した報酬、未使用の期間、直前の演算子からの遷移報酬の和が最大となる演算子を選ぶ Choice Function 型ハイパーヒューリスティクスである。改善したら報酬 1、改善しなければ 0 として更新する。`reaction_factor` は $(0,1]$、`recency_weight` と `transition_weight` は非負でなければならない。

時間計算量は $O(E+I(H+A+E))$、追加空間計算量は遷移表の $O(H^2)$ である。

## 共通操作の計算量

`SearchResult` の各メンバの参照は時間・追加空間とも $O(1)$ である。

## 注意点

- `heuristics` が空の場合やパラメータが上記の条件を満たさない場合は `std::invalid_argument` を送出する
- 演算子の range は探索中に変更してはならず、各演算子が返す状態は独立でなければならない
- Choice Function の報酬は目的値の改善量ではなく改善の有無である。尺度を考慮した報酬が必要なら、低レベルの演算子の側で正規化する
- ランダム選択も学習型の選択も有限の計算量の範囲で動くヒューリスティックであり、大域最適性は保証しない
