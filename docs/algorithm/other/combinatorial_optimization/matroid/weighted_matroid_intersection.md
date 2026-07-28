---
title: Weighted Matroid Intersection (重み付きマトロイド交差)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/matroid/weighted_matroid_intersection.hpp
---

2マトロイドの共通独立集合のうち、重み合計が最大の集合を求める。各cardinalityで最大重みの交換路を選び、全cardinality中の最良解を返す。

## `WeightedMatroidIntersectionResult<Weight>`

`elements` が選択要素、`weight` が合計重み。全重みが負なら空集合を返せる。

## `weighted_matroid_intersection(weights, first_independent, second_independent)`

最大rankを $r$、要素数を $n$、oracle時間を $T$ とすると、交換グラフ構築とBellman–Ford型最大gain路により時間計算量 $O(r(n^2T+n^3))$、空間計算量 $O(n^2)$。同gainでは辺数が短い交換路を選び、交換路の独立性を保つ。

## 注意点

`Weight` は符号付き加法・単項minus・全順序を提供する必要がある。oracleはマトロイド公理を満たすこと。空集合を拒否した場合は `std::invalid_argument` を送出する。
