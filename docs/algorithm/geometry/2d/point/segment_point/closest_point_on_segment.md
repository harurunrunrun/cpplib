---
title: Closest Point on Segment (線分上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/segment_point/closest_point_on_segment.hpp
---

## API

`Point closest_point_on_segment(segment, point)`

## 引数

線分 `segment` と照会点 `point` を渡す。

## 戻り値

線分上の点のうち、照会点からの距離が最小となる点。

## API別の時間計算量・空間計算量

- `closest_point_on_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

線分の長さが `GEOMETRY_EPS` 以下の退化線分では、端点 `segment.a` を返す。
