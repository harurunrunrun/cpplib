---
title: Convex Polyhedron Support Plane (凸多面体の支持平面)
documentation_of: ../src/algorithm/geometry/3d/shape/convex_polyhedron_support_plane.hpp
---

## API

- `convex_polyhedron_support_plane(polyhedron, outward_normal)`: 指定法線に対する支持点を通る支持平面を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として時間 $O(V)$、追加領域 $O(1)$。

## 注意点

法線は正規化されない。入力検査と例外は `convex_polyhedron_support_point` に従う。
