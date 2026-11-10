---
title: Weighted Matroid Intersection (重み付きマトロイド交差)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/matroid/weighted_matroid_intersection.hpp
---

2 つのマトロイドの共通独立集合のうち、重みの合計が最大の集合を求める。各要素数で重みの増分が最大の交換路を選んで要素数を $1$ ずつ増やし、全要素数のうち最良の解を返す。名前空間は `combinatorial_optimization` である。

## `WeightedMatroidIntersectionResult<Weight>`

`elements` が選んだ要素（昇順）、`weight` が重みの合計である。全ての重みが負なら空集合（`weight` は `Weight{}`）を返しうる。

## `weighted_matroid_intersection(weights, first_independent, second_independent)`

`weights[i]` が要素 $i$ の重みで、台集合の大きさは `weights.size()` である。各独立性オラクルは要素番号を昇順に並べた `std::vector<std::size_t>` を受け取る。

## 時間計算量

最大共通独立集合の大きさを $r$、要素数を $n$、オラクル 1 回の時間を $T$ とする。各段階で交換グラフを構築し（オラクル $O(n^2)$ 回）、Bellman–Ford 法で重みの増分が最大の路を求める（$O(n^3)$）ため、時間計算量は $O(r(n^2(T + n \log n)+n^3))$ である。増分が等しい路の中では辺数が少ないものを選び、交換後の集合の独立性を保つ。

## 空間計算量

$O(n^2)$（交換グラフ）。

## 注意点

- `Weight` は加算、単項マイナス、`<` による全順序、デフォルト構築による $0$ を提供する必要がある。
- オラクルはマトロイドの公理を満たす必要がある。空集合を拒否した場合は `std::invalid_argument` を送出する。
