---
title: Segment-Point Closest Point (線分上の点への最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/segment_point/segment_point_closest_point.hpp
---

## API

- `closest_point(segment, point)`: 点に最も近い閉線分上の点を返す。

## API別の時間計算量・空間計算量

- 対応API: 時間・追加領域 $O(1)$。

## 注意点

退化線分も一点として扱う。座標は有限値でなければならない。
