---
title: Convex Polyhedron Support Plane (凸多面体の支持平面)
documentation_of: ../../../../../../src/algorithm/geometry/3d/plane/polyhedron_point/convex_polyhedron_support_plane.hpp
---

## API

- `convex_polyhedron_support_plane(polyhedron, outward_normal)`: `outward_normal` 方向の支持点（`convex_polyhedron_support_point` の戻り値）を通り、`outward_normal` を法線とする支持平面を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として時間 $O(V)$、追加領域 $O(1)$（exact dyadic 演算の桁領域を除く）。

## 注意点

- 法線は正規化されず、`outward_normal` がそのまま平面の法線になる。
- 入力検査と例外は `convex_polyhedron_support_point` に従う。
