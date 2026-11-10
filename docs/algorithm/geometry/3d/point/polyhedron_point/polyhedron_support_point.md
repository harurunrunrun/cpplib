---
title: General Polyhedron Support Point (一般多面体の支持点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polyhedron_point/polyhedron_support_point.hpp
---

## API

- `polyhedron_support_point(polyhedron, direction)`: `direction` との内積が最大となる頂点の座標を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として時間 $O(V)$、追加領域 $O(1)$（exact dyadic 演算の桁領域を除く）。

## 注意点

- 凸性は要求しない。
- 入力検査、同値時の選択、例外は `polyhedron_support_vertex` に従う（空の頂点集合・零方向・非有限値には `std::invalid_argument`）。
