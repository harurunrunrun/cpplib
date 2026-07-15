---
title: Convex Polyhedron Plane Section (凸多面体の平面断面)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_plane_section.hpp
---

## API

- `convex_polyhedron_plane_section(polyhedron, plane)`: 平面との共通部分の頂点を平面上の凸順序で返す。空・点・線分も頂点列で表す。

## API別の時間計算量・空間計算量

- 辺数を $E$、交点数を $K$ として時間 $O(E\log E+K^2)$、領域 $O(E+K)$。

## 注意点

ゼロ法線と非有限入力を拒否する。共面する三角形分割対角線は辺列挙時に除外する。
