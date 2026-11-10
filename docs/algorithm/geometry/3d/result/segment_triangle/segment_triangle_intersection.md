---
title: 3D Segment and Triangle Intersection (三次元線分と三角形の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/segment_triangle/segment_triangle_intersection.hpp
---

三次元の閉線分と閉三角形の共通部分を求める。

## API

- `segment_triangle_intersection(segment, triangle)`: 2 つの閉集合の共通部分を `LinearIntersection3` として、空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`）のいずれかで返す。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

- 退化した線分（両端点がほぼ一致）は一点として扱い、点の包含判定を行う。
- 退化していない線分に対して、退化した三角形には `std::invalid_argument` を送出する。
