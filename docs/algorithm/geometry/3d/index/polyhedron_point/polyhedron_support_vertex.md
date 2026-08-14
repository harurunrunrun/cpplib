---
title: General Polyhedron Support Vertex (一般多面体の支持頂点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index/polyhedron_point/polyhedron_support_vertex.hpp
---

凸性や面の接続を仮定せず、指定方向への内積が最大の頂点indexを返す。

## API

- `polyhedron_support_vertex(polyhedron, direction)`: 支持頂点のindexを返す。同値なら最小indexを返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として時間計算量 $O(V)$、追加空間計算量 $O(1)$（exact dyadic整数の桁領域を除く）。

## 注意点

空形状、ゼロ方向、非有限値には `std::invalid_argument` を送出する。比較はexact dyadic内積で行い、丸めで最大頂点を取り違えない。
