---
title: Routing Tour and Path Cost (巡回路・経路コスト)
documentation_of: ../src/approximate/routing/tour_cost.hpp
---

距離行列上の巡回路・経路のコストを求める。APIは `approximate::routing` 名前空間にある。
巡回路は始点を末尾に重複して持たない頂点列で表す。

## `tour_cost`

~~~cpp
template<class DistanceMatrix>
Cost tour_cost(const DistanceMatrix& distance, const vector<int>& tour)
~~~

`tour[i]` から `tour[(i+1)%tour.size()]` までを合計する。空巡回路のコストは `Cost{}`。
`distance` は `size()` と `operator[][]` を持つ正方距離行列、`Cost` はその要素型である。

- 時間計算量: 行列検査を含めて $O(N+K)$
- 追加空間計算量: $O(1)$

## `path_cost`

~~~cpp
template<class DistanceMatrix>
Cost path_cost(const DistanceMatrix& distance, const vector<int>& path)
~~~

隣接する頂点間だけを合計し、末尾から先頭へは戻らない。空経路と1頂点経路のコストは `Cost{}`。

- 時間計算量: 行列検査を含めて $O(N+K)$
- 追加空間計算量: $O(1)$

## 注意点

`N=distance.size()`, `K=tour.size()` または `path.size()` とする。
非正方行列には `std::invalid_argument`、範囲外頂点には `std::out_of_range` を送出する。
加算結果は `Cost` に収まらなければならない。
