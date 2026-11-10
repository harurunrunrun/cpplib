---
title: Exact Unique Segment Intersection (厳密線分単一交点)
documentation_of: ../../../../../src/algorithm/integer_geometry/result/segment_segment/segment_intersection_point.hpp
---

2 本の線分の共有部分がちょうど 1 点ならその点を返す。

## API

- `segment_intersection_point(first, second)`: 共有点がただ 1 つなら `std::optional<Point>` としてその点を返す。共有点がない場合と、共線で正の長さにわたって重なる場合は `std::nullopt` を返す。端点で接する場合はその端点を返す。

## 時間計算量・空間計算量

- `segment_intersection_point(first, second)`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
