---
title: Distance between 3D Plane and Segment in Plane-Segment Order (平面・線分順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_segment/segment_plane_distance_plane_segment.hpp
---

## API

- `distance(plane, segment)`: `Plane3` と `Segment3` の集合間距離を返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

逆順leafは `distance(segment, plane)` へ委譲する。返り値と例外条件は順方向leafと同じである。
