---
title: General Polyhedron Support Point (一般多面体の支持点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polyhedron_point/polyhedron_support_point.hpp
---

## API

- `polyhedron_support_point(polyhedron, direction)`: 指定方向への内積が最大の頂点座標を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として時間計算量 $O(V)$、追加空間計算量 $O(1)$（exact dyadic整数の桁領域を除く）。

## 注意点

凸性は要求しない。入力検査、同値時の選択、例外は `polyhedron_support_vertex` に従う。
