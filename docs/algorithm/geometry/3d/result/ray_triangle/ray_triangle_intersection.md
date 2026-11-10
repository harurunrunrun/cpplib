---
title: 3D Ray and Triangle Intersection (三次元半直線と三角形の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/ray_triangle/ray_triangle_intersection.hpp
---

三次元の閉半直線と閉三角形の共通部分を求める。

## API

- `ray_triangle_intersection(ray, triangle)`: 共通部分を `LinearIntersection3` として、空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`）のいずれかで返す。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

- 半直線が三角形と同一平面上にある場合も扱う（共通部分は線分になり得る）。
- 退化した半直線・退化した三角形には `std::invalid_argument` を送出する。
