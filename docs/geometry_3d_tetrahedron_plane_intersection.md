---
title: 3D Tetrahedron Plane Section (三次元四面体の平面切断)
documentation_of: ../src/algorithm/geometry/3d/query/tetrahedron_plane_intersection.hpp
---

## API

- `tetrahedron_plane_intersection(tetrahedron, plane)`: 切断集合を空、点、線分、巡回順の三角形または四角形 `Polygon3` として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化四面体または零法線には `std::invalid_argument` を送出する。
