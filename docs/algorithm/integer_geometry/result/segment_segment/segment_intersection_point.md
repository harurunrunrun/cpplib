---
title: Exact Unique Segment Intersection (厳密線分単一交点)
documentation_of: ../../../../../src/algorithm/integer_geometry/result/segment_segment/segment_intersection_point.hpp
---

二線分の交わりが一点だけならその点を返す。

## API

- `segment_intersection_point(first, second)`: 一意な共有点、または `nullopt` を返す。

## API別の時間計算量・空間計算量

- `segment_intersection_point(first, second)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
