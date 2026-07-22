---
title: 3D Triangle and Plane Intersection (三次元三角形と平面の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/triangle_plane_intersection.hpp
---

## API

- `triangle_plane_intersection(triangle, plane)`: 共通部分を空、点、線分、三角形を格納した `Polygon3` として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

三角形全体が平面上なら3頂点の `Polygon3` を返す。退化三角形または零法線には `std::invalid_argument` を送出する。
