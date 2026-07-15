---
title: Convex Polyhedron Counts (凸多面体の要素数)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_counts.hpp
---

## API

- `ConvexPolyhedronCounts3`: `vertices`, 幾何学的 `edges`, 幾何面数 `facet_count`, 格納三角形数 `triangulated_face_count` を保持する。`triangular_faces` は後方互換のため `triangulated_face_count` と同じ値を保持する。
- `convex_polyhedron_counts(polyhedron)`: 頂点・幾何辺・幾何面・三角形分割後の面の各個数を返す。

## API別の時間計算量・空間計算量

- `convex_polyhedron_counts`: 格納三角形数を $F$ として時間 $O(F\log F)$、空間 $O(F)$。

## 注意点

同一平面上の三角形間の内部対角線は `edges` に含めず、同一平面で連結な三角形群を `facet_count` では1面と数える。不正な面添字・非多様体形状に対しては、利用する辺・面列挙APIと同じ例外を送出する。
