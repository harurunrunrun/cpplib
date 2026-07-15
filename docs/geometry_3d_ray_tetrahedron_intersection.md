---
title: 3D Ray and Tetrahedron Intersection (三次元半直線と四面体の共通部分)
documentation_of: ../src/algorithm/geometry/3d/ray_tetrahedron_intersection.hpp
---

## API

- `ray_tetrahedron_intersection(ray, tetrahedron)`: 共通部分を空、点、線分として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化半直線・退化四面体には `std::invalid_argument` を送出する。
