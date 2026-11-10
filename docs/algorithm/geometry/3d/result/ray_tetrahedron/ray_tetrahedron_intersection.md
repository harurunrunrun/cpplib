---
title: 3D Ray and Tetrahedron Intersection (三次元半直線と四面体の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/ray_tetrahedron/ray_tetrahedron_intersection.hpp
---

三次元の半直線と閉四面体の共通部分を求める。

## API

- `ray_tetrahedron_intersection(ray, tetrahedron)`: 共通部分を `LinearIntersection3` として、空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`）のいずれかで返す。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

退化した半直線・退化した四面体には `std::invalid_argument` を送出する。
