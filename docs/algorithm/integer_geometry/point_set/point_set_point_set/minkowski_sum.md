---
title: Exact Minkowski Sum (厳密Minkowski和)
documentation_of: ../../../../../src/algorithm/integer_geometry/point_set/point_set_point_set/minkowski_sum.hpp
---

二点集合または二凸多角形のMinkowski和の凸包を返す。

## API

- `minkowski_sum_convex(first, second)`: 凸多角形同士を辺ベクトルのマージで加算する。
- `minkowski_sum(first, second)`: 一般の点集合を凸包化してからMinkowski和を返す。

## API別の時間計算量・空間計算量

- `minkowski_sum_convex(first, second)`: 時間 O(n + m)、戻り値を含む追加領域 O(n + m)。
- `minkowski_sum(first, second)`: 時間 O(n log n + m log m)、戻り値を含む追加領域 O(n + m)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
