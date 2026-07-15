---
title: Convex Polyhedron Plane Section (凸多面体の平面断面)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_plane_section.hpp
---

## API

- `convex_polyhedron_plane_section(polyhedron, plane)`: 平面との共通部分の頂点を平面上の凸順序で返す。空・点・線分も頂点列で表す。

## API別の時間計算量・空間計算量

- 辺数を $E$、交点数を $K$ として時間 $O(E+K\log K)$、領域 $O(E+K)$。

## 注意点

ゼロ法線と非有限入力を拒否する。共面する三角形分割対角線は辺列挙時に除外する。
頂点の側判定、辺交点、重複除去、凸順序はraw法線とexact dyadic predicateで処理するため、
巨大な平行移動に比べて非常に薄い断面も保持する。
