---
title: Closest Points for 3D Plane and Segment in Plane-Segment Order (平面・線分順の三次元最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/plane_segment/segment_plane_closest_points_plane_segment.hpp
---

## API

- `closest_points(plane, segment)`: `Plane3` 上の点と `Segment3` 上の点を、引数と同じ順の `std::pair<Point3, Point3>` で返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

`closest_points(segment, plane)` の結果を入れ替えて返すだけなので、最近点の選び方と例外条件は `closest_points(segment, plane)` と同じである。
