---
title: 3D Triangle and Plane Intersection (三次元三角形と平面の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/triangle_plane/triangle_plane_intersection.hpp
---

## API

- `triangle_plane_intersection(triangle, plane)`: 三角形と平面の共通部分を `PlanarIntersection3` として返す。空なら `std::monostate`、1 点なら `Point3`、線分なら `Segment3`、三角形全体なら 3 頂点を格納した `Polygon3` になる。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 三角形全体が平面上にある場合は、3 頂点 `a`, `b`, `c` をこの順に格納した `Polygon3` を返す。
- 退化した三角形、または平面の法線が零ベクトルの場合は `std::invalid_argument` を送出する。
