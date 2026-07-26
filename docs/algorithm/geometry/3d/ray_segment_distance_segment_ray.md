---
title: Distance between 3D Segment and Ray in Segment-Ray Order (線分・半直線順の三次元距離)
documentation_of: ../../../../src/algorithm/geometry/3d/ray_segment_distance_segment_ray.hpp
---

## API

- `distance(segment, ray)`: `Segment3` と `Ray3` の集合間距離を返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

逆順leafは `distance(ray, segment)` へ委譲する。返り値と例外条件は順方向leafと同じである。
