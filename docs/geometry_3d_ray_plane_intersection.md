---
title: 3D Ray-Plane Intersection (三次元半直線と平面の交点)
documentation_of: ../src/algorithm/geometry/3d/ray_plane_intersection.hpp
---

## API

- `ray_plane_intersection(ray, plane)`: 半直線上の一意な交点を返し、存在しないか無数なら `nullopt`。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化半直線・零法線には `std::invalid_argument` を送出する。始点上の交点を含む。
