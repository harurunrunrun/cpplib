---
title: 3D Segment and Tetrahedron Intersection (三次元線分と四面体の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/segment_tetrahedron/segment_tetrahedron_intersection.hpp
---

三次元の線分と閉四面体の共通部分を求める。

## API

- `segment_tetrahedron_intersection(segment, tetrahedron)`: 共通部分を `LinearIntersection3` として、空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`）のいずれかで返す。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

- 退化した線分（両端点がほぼ一致）は一点として扱い、点の包含判定を行う。
- 退化していない線分に対して、退化した四面体には `std::invalid_argument` を送出する。
