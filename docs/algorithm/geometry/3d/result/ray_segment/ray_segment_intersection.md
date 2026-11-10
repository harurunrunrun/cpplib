---
title: 3D Ray and Segment Intersection (三次元半直線と線分の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/ray_segment/ray_segment_intersection.hpp
---

三次元の半直線と線分の共通部分を求める。

## API

- `ray_segment_intersection(ray, segment)`: 共通部分を `LinearIntersection3` で返す。空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`）のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

- 退化した線分（両端点がほぼ一致）は一点として扱う。
- 退化した半直線（`origin` と `through` の距離が `GEOMETRY3D_EPS` 以下）には `std::invalid_argument` を送出する。
