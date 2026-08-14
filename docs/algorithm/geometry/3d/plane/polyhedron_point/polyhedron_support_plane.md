---
title: General Polyhedron Support Plane (一般多面体の支持平面)
documentation_of: ../../../../../../src/algorithm/geometry/3d/plane/polyhedron_point/polyhedron_support_plane.hpp
---

凸性を仮定せず、全頂点を指定法線の負側に含む支持平面を返す。

## API

- `polyhedron_support_plane(polyhedron, outward_normal)`: 支持頂点を通り、`outward_normal` を法線とする平面を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として時間計算量 $O(V)$、追加空間計算量 $O(1)$（exact dyadic整数の桁領域を除く）。

## 注意点

空形状、ゼロ法線、非有限値の扱いは `polyhedron_support_point` に従う。非凸形状でも形状全体の支持平面として定義される。
