---
title: Closest Points for 3D Segment and Ray in Segment-Ray Order (線分・半直線順の三次元最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/segment_ray/ray_segment_closest_points_segment_ray.hpp
---

## API

- `closest_points(segment, ray)`: `Segment3` 上の点と `Ray3` 上の点を、引数と同じ順の `std::pair<Point3, Point3>` で返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

`closest_points(ray, segment)` の結果を入れ替えて返すだけなので、最近点の選び方と例外条件は `closest_points(ray, segment)` と同じである。
