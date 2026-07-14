---
title: Closest Point on Segment (線分上の最近点)
documentation_of: ../src/algorithm/geometry/2d/closest_point_on_segment.hpp
---

## API

`Point closest_point_on_segment(segment, point)`

## 引数

線分と照会点を渡す。

## 戻り値

照会点からの距離を最小にする線分上の点。

## API別の時間計算量・空間計算量

- `closest_point_on_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化線分ではその端点を返す。
