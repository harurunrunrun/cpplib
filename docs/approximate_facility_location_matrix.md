---
title: Distance-Matrix Facility Location Heuristics (距離行列による施設配置ヒューリスティック)
documentation_of: ../src/approximate/facility/facility_location.hpp
---

## 施設配置ヒューリスティック（距離行列API）

既存の `approximate::clustering` の施設配置実装を、`distance[facility][client]` の行列から呼び出すadapter。アルゴリズム本体は重複実装しない。

## `greedy_facility_location(distance, opening_cost)`

最良の単一施設から始め、開設費と割当距離の合計を改善する施設を貪欲に追加する。`FacilityLocationResult` の `labels[c]` は施設IDではなく `facilities` 内の位置を表す。`KMedianResult::assignment` が施設IDそのものを格納する点とは異なる。

- 時間計算量: 最悪 $O(F^3C)$
- 追加空間計算量: $O(FC+F+C)$
- 近似: 一般の距離行列では近似比を保証しない。

## `add_drop_facility_location(distance, opening_cost, maximum_iterations)`

貪欲解から、施設1個の追加または削除で費用を改善する操作を反復する。施設をすべて削除する操作は行わない。

- 時間計算量: $O(F^3C+IF^2C)$
- 追加空間計算量: $O(FC+F+C)$
- 近似: 一般の距離行列では近似比を保証しない。

行列は1行以上の長方形で、距離と開設費は有限かつ非負でなければならない。開設費は候補施設ごとに1個必要である。目的値が `long double` の範囲を超える場合は例外を送出する。

## 注意点

施設・需要点の添字、距離行列の形状、距離の有限性と非負性は各APIの条件に従う。明記した場合を除き、局所探索が大域最適解や一定の近似比を返す保証はない。
