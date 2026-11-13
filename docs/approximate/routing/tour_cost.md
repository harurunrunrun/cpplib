---
title: Routing Tour and Path Cost (巡回路・経路コスト)
documentation_of: ../../../src/approximate/routing/tour_cost.hpp
---

距離行列上の巡回路・経路のコストを求める。API は `approximate::routing` 名前空間にある。
巡回路は、始点を末尾に重複して持たない頂点列で表す。

以下、$N$ を `distance.size()`、$K$ を `tour.size()` または `path.size()` とする。

## `tour_cost`

~~~cpp
template<class DistanceMatrix>
Cost tour_cost(const DistanceMatrix& distance, const vector<int>& tour)
~~~

`tour[i]` から `tour[(i + 1) % tour.size()]` への距離を合計する。空の巡回路のコストは `Cost{}`。
`distance` は `size()` と 2 段の `operator[]` を持つ正方の距離行列で、`Cost` はその要素型である。

- 時間計算量: 行列の検査を含めて $O(N + K)$
- 追加空間計算量: $O(1)$

## `path_cost`

~~~cpp
template<class DistanceMatrix>
Cost path_cost(const DistanceMatrix& distance, const vector<int>& path)
~~~

隣接する頂点間の距離だけを合計し、末尾から先頭へは戻らない。空の経路と 1 頂点の経路のコストは `Cost{}`。

- 時間計算量: 行列の検査を含めて $O(N + K)$
- 追加空間計算量: $O(1)$

## 注意点

- 正方でない行列には `std::invalid_argument` を、範囲外の頂点には `std::out_of_range` を送出する。
- 加算結果は `Cost` に収まらなければならない。
