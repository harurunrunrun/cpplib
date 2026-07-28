---
title: Triangle-Point Closest Point (三角形上の点への最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/triangle_point/triangle_point_closest_point.hpp
---

## API

- `closest_point(triangle, point)`: 点に最も近い閉三角形領域上の点を返す。

## API別の時間計算量・空間計算量

- 対応API: 時間・追加領域 $O(1)$。

## 注意点

退化三角形または非有限入力では例外を送出する。
