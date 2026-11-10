---
title: 3D Tetrahedron Plane Section (三次元四面体の平面切断)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/tetrahedron_plane/tetrahedron_plane_intersection.hpp
---

## API

- `tetrahedron_plane_intersection(tetrahedron, plane)`: 四面体を平面で切った切断集合を `PlanarIntersection3` として返す。空なら `std::monostate`、1 点なら `Point3`、線分なら `Segment3`、三角形または四角形なら頂点を巡回順に並べた `Polygon3` になる。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 各頂点の平面への符号付き距離を許容誤差付きで符号判定し、平面上の頂点と、平面をまたぐ辺の交点を集める。
- 多角形の頂点は重心まわりの偏角で並べ替える。
- 退化した四面体、または平面の法線が零ベクトルの場合は `std::invalid_argument` を送出する。
