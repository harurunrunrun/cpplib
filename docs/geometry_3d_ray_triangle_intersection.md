---
title: 3D Ray and Triangle Intersection (三次元半直線と三角形の共通部分)
documentation_of: ../src/algorithm/geometry/3d/ray_triangle_intersection.hpp
---

## API

- `ray_triangle_intersection(ray, triangle)`: 閉半直線と閉三角形の共通部分を空、点、線分として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

共面ケースを含む。退化半直線・退化三角形には `std::invalid_argument` を送出する。
