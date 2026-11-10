---
title: Convex Polyhedron Counts (凸多面体の要素数)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_counts.hpp
---

凸多面体の頂点・辺・面の個数をまとめて求める。

## API

- `convex_polyhedron_counts(polyhedron)`: `ConvexPolyhedronCounts3` を返す。各メンバは次のとおり。
  - `vertices`: `polyhedron.vertices` の要素数。
  - `edges`: 幾何的な辺の数（`convex_polyhedron_edges` の要素数）。
  - `facet_count`: 幾何的な面の数（`convex_polyhedron_facets` の要素数）。
  - `triangulated_face_count`: 三角形分割後の面の数（`polyhedron.faces` の要素数）。
  - `triangular_faces`: `triangulated_face_count` と同じ値（後方互換のための別名）。

## API別の時間計算量・空間計算量

- `convex_polyhedron_counts`: 格納されている三角形の数を $F$ とすると、時間 $O(F \log F)$、空間 $O(F)$。

## 注意点

- 同一平面上の三角形の間にある内部対角線は `edges` に含めない。同一平面上で連結な三角形群は `facet_count` では 1 面と数える。
- 不正な面添字や非多様体の形状に対しては、内部で利用する辺・面列挙 API（`convex_polyhedron_edges`, `convex_polyhedron_facets`）と同じ例外を送出する。
