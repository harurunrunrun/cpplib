---
title: Exact 2D Geometry Types (厳密二次元幾何型)
documentation_of: ../../../../../src/algorithm/integer_geometry/type/definition/geometry_types.hpp
---

有理座標のベクトル、点、直線、線分と交差・内外判定の列挙型を定義する。

## API

- `Vector`, `Point`, `Line`, `Segment`: 幾何要素を保持する。
- `SegmentIntersectionKind`, `PointLocation`: 判定結果を表す。

## API別の時間計算量・空間計算量

- `Vector`, `Point`, `Line`, `Segment`: 時間 構築・比較とも O(1)、追加領域 O(1)。
- `SegmentIntersectionKind`, `PointLocation`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
