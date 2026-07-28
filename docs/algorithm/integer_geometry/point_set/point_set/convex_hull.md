---
title: Exact Convex Hull (厳密凸包)
documentation_of: ../../../../../src/algorithm/integer_geometry/point_set/point_set/convex_hull.hpp
---

点集合の凸包頂点を反時計回りに列挙する。

## API

- `convex_hull(points)`: 重複点と辺上の不要点を除いた凸包を返す。

## API別の時間計算量・空間計算量

- `convex_hull(points)`: 時間 O(n log n)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
