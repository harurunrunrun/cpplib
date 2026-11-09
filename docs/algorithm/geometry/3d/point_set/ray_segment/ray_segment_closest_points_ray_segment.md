---
title: Closest Points for 3D Ray and Segment in Ray-Segment Order (半直線・線分順の三次元最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/ray_segment/ray_segment_closest_points_ray_segment.hpp
---

## API

- `closest_points(ray, segment)`: `Ray3` 上の点と `Segment3` 上の点を、引数と同じ順の `std::pair<Point3, Point3>` で返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 線分は退化（両端点が一致）していてもよい。
- 退化した半直線、非有限な入力には `std::invalid_argument` を、結果が表現できない場合は `std::overflow_error` を送出する。
- 最近点対が複数ある場合は、そのうち 1 組を返す。
