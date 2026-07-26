---
title: Segment-Point Distance (線分と点の距離)
documentation_of: ../../../../src/algorithm/geometry/2d/segment_point_distance.hpp
---

閉線分と点の最短距離を求める。

## API

- `distance_segment_point(segment, point)`: `segment` と `point` の距離を返す。

## API別の時間計算量・空間計算量

- `distance_segment_point`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

長さ0の退化線分では端点と点の距離を返す。戻り値の型は `long double` である。
