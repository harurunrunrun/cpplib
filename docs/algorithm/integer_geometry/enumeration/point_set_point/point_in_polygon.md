---
title: Exact Point in Polygon (厳密点多角形内外判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/enumeration/point_set_point/point_in_polygon.hpp
---

点を多角形の外部・境界・内部に分類する。

## API

- `point_in_polygon(polygon, point)`: 点の位置を `PointLocation` で返す。

## API別の時間計算量・空間計算量

- `point_in_polygon(polygon, point)`: 時間 O(n)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
