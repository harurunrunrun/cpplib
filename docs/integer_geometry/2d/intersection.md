---
title: Exact 2D Intersections (厳密二次元交点)
documentation_of: ../../../src/integer_geometry/2d/intersection.hpp
---

直線および線分の交点を有理数座標で厳密に求める。

## API

- `line_intersection(first, second)`: 一意な直線交点を返す。平行または一致なら `std::nullopt`。
- `segment_intersection_point(first, second)`: 交点が一意なら返し、非交差または区間重複なら `std::nullopt`。
- `segment_intersection(first, second)`: 共通部分を0点、1点、または重複区間の2端点として返す。

## API別の時間計算量・空間計算量

座標の最大bit長を $L$、有理数演算時間を $R(L)$ とする。

- 全API: 時間 $O(R(L))$、追加領域 $O(L)$。

## 注意点

`line_intersection` は平行と一致を区別しない。重複線分の2端点は辞書順で返す。
